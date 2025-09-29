#include <platform/hal/simulator/sdl2/HALSDL2.hpp>
#include <touchgfx/hal/NoDMA.hpp>
#include <common/TouchGFXInit.hpp>
#include <gui_generated/common/SimConstants.hpp>
#include <platform/driver/touch/SDL2TouchController.hpp>
#include <platform/driver/touch/NoTouchController.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <simulator/mainBase.hpp>
#include "touchgfx/Utils.hpp"

#include "SDK/Simulator/Kernel/KernelBase.hpp"
#include "SDK/Kernel/KernelProviderGUI.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"
#include "SDK/Platform/OS/OS.hpp"
#include "SDK/Simulator/Sensors/SensorCore.hpp"
#include "SDK/Simulator/Kernel/Mock/MockServiceControl.hpp"
#include "SDK/AppSystem/SvcBootstrap.hpp"

#include "gui/common/GuiConfig.hpp"
#include "Service.hpp"

#include <stdlib.h>
#include <thread>

#ifdef __linux__
#include <time.h>
#else
#include "Windows.h"
#endif

#define LOG_MODULE_PRX      "main::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

using namespace touchgfx;

static uint32_t LoggerTimeFunc(void)
{
#ifdef __linux__
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (uint32_t)((ts.tv_sec * 1000ULL) + (ts.tv_nsec / 1000000ULL));
#else
    return (uint32_t)GetTickCount64();
#endif
}

static void LoggerPrint(const char* str)
{
    touchgfx_printf(str);
}

// Service thread function
static void runService(SDK::Service::Bootstrap* bootstrap)
{
    LOG_INFO("service thread is started\n");

    bootstrap->run();
}

static int runTouchGFX(Simulator::KernelBase& serviceKernel, Simulator::KernelBase& guiKernel, int argc, char** argv)
{
    // Init KernelHolder and KernelManager
    Simulator::KernelHolder::Create(guiKernel);
    SDK::KernelProviderGUI::CreateInstance(guiKernel.getKernel());

    // Create a service app object and call approproate callbacks
    SDK::KernelProviderService::CreateInstance(serviceKernel.getKernel());
    //Service service;
    SDK::Service::Bootstrap bootstrap;

    //For windows/linux, DMA transfers are simulated
    touchgfx::NoDMA dma;
    LCD& lcd = setupLCD();
    touchgfx::NoTouchController tc;

    touchgfx::HAL& hal = touchgfx::touchgfx_generic_init<touchgfx::HALSDL2>(dma, lcd, tc, SIM_WIDTH, SIM_HEIGHT, 0, 0);

    setupSimulator(argc, argv, hal);

    // Set custom frame rate
    static_cast<touchgfx::HALSDL2&>(hal).setVsyncInterval(1000.0f / Gui::Config::kFrameRate);

    //// Ensure there is a console window to print to using printf() or
    //// std::cout, and read from using e.g. fgets or std::cin.
    //// Alternatively, instead of using printf(), always use
    //// touchgfx_printf() which will ensure there is a console to write
    //// to.
    touchgfx_enable_stdio();

    // Start service thread
    serviceKernel.startApp();
    std::thread serviceThread(runService, &bootstrap);

    guiKernel.startApp();                           // Start GUI kernel simulator
    touchgfx::HAL::getInstance()->taskEntry();      // Main GUI loop
    guiKernel.stopApp();                            // Stop GUI kernel simulator

    // Stop service kernel simulator and stop its thread
    serviceKernel.stopApp();
    serviceThread.join();

    return EXIT_SUCCESS;
}

#ifdef __linux__
int main(int argc, char** argv)
{
#else
#include <shellapi.h>
    #ifdef _UNICODE
    #error Cannot run in unicode mode
    #endif
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int argc;
    char** argv = touchgfx::HALSDL2::getArgv(&argc);
#endif

    OS::Mutex mutexLog;
    Logger_init(LoggerPrint);
    Logger_setMutex(&mutexLog);
    Logger_setTimeFunc(LoggerTimeFunc);

    // Create kernel objects and service control
    Sensor::Core                  sensorCore;
    Simulator::MockServiceControl serviceControl;
    Simulator::KernelBase         serviceKernel(true, serviceControl);
    Simulator::KernelBase         guiKernel(false, serviceControl, &sensorCore);

    return runTouchGFX(serviceKernel, guiKernel, argc, argv);
}
