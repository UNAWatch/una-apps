#include <platform/hal/simulator/sdl2/HALSDL2.hpp>
#include <touchgfx/hal/NoDMA.hpp>
#include <common/TouchGFXInit.hpp>
#include <gui_generated/common/SimConstants.hpp>
#include <platform/driver/touch/SDL2TouchController.hpp>
#include <platform/driver/touch/NoTouchController.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <simulator/mainBase.hpp>
#include "touchgfx/Utils.hpp"

#include "SDK/Kernel/KernelBuilder.hpp"
#include "SDK/Simulator/Kernel/Kernel.hpp"
#include "SDK/Simulator/Sensors/SensorCore.hpp"
#include "SDK/Kernel/KernelProviderGUI.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"
#include "SDK/Simulator/OS/OS.hpp"

#include "gui/common/GuiConfig.hpp"
#include "Service.hpp"

#include <stdlib.h>
#include <thread>

#ifdef __linux__
#include <time.h>
#else
#include "Windows.h"
#endif

#define LOG_MODULE_PRX      "main"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

using namespace touchgfx;

// Kernel thread function
static void kernelThreadFunction(void* obj)
{
    //service->run();
}

// Service thread function
static void serviceThreadFunction(Service* service)
{
    service->run();
}

static int runTouchGFX(SDK::Simulator::Kernel& srvIKernel,
                       SDK::Simulator::Kernel& guiIKernel,
                       int                     argc,
                       char**                  argv)
{
    // Make kernel facede for 'Service'
    SDK::Kernel srvKernel = SDK::KernelBuilder::make(srvIKernel.getInterface());

    // Save Service's kernel for global access
    SDK::KernelProviderService::CreateInstance(&srvKernel);

    // Make kernel facede for 'GUI'
    SDK::Kernel guiKernel = SDK::KernelBuilder::make(guiIKernel.getInterface());

    // Save GUI's kernel for global access
    SDK::KernelProviderGUI::CreateInstance(&guiKernel);

    // Create the Service of the application
    Service  service(SDK::KernelProviderService::GetInstance().getKernel());

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

    // Initialize Logger with Service's kernel. In real app Service and GUI will have each its own kernel.
    Logger_init(srvKernel.log);

    // Start service thread
    srvIKernel.startApp();
    std::thread serviceThread(serviceThreadFunction, &service);
    std::thread kernelThread(kernelThreadFunction, nullptr);

    guiIKernel.startApp();                      // Start GUI kernel simulator
    touchgfx::HAL::getInstance()->taskEntry();  // Main GUI loop
    guiIKernel.stopApp();                       // Stop GUI kernel simulator

    // Stop service kernel simulator and stop its thread
    srvIKernel.stopApp();
    serviceThread.join();

    kernelThread.join();

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

    // Create kernel objects and service control
    SDK::Simulator::Sensors::Core sensorCore;

    SDK::Simulator::Kernel serviceKernel("service", nullptr);

    // GUI Kernel handle sensors as it has tick() from tiuchgfx
    SDK::Simulator::Kernel guiKernel("gui", &sensorCore);

    // Save GUI kernel for simulator
    SDK::Simulator::KernelHolder::Create(guiKernel);

    return runTouchGFX(serviceKernel, guiKernel, argc, argv);
}
