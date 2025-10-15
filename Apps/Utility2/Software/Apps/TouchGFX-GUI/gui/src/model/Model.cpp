#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <gui/common/FrontendApplication.hpp>
#include "SDK/Kernel/KernelProviderGUI.hpp"

#define LOG_MODULE_PRX      "Model"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#if defined(SIMULATOR)
    #include "touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp"
    #include "Windows.h"
    #include <chrono>
    #include <ctime>
#endif

Model::Model()
    : mKernel(SDK::KernelProviderGUI::GetInstance().getKernel())
    , modelListener(0)
    , mGSModel(std::static_pointer_cast<IGUIModel>(mKernel.gctrl.getContext()))
    , mCounter(0)
{
    mKernel.app.registerApp(this);
    mGSModel->setGUIHandler(&mKernel, this);

    LOG_INFO("GUI [Utility #2] is initialized\n");

#if defined(SIMULATOR)
    LOG_DEBUG("Application is running through simulator! \n");

    std::string fileStoreDir = Simulator::KernelHolder::Get().getFsPath();
    LOG_DEBUG("Path to files created by app:\n   [%s]\n", fileStoreDir.c_str());

    LOG_DEBUG_WP("\n"
        "       Keys:                       \n"
        "       ----------------------------\n"
        "       1   L1,                     \n"
        "       2   L2,                     \n"
        "       3   R1,                     \n"
        "       4   R2,                     \n"
        "       z   L1+R2                   \n"
    );
#endif
}

FrontendApplication& Model::application()
{
    return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
}

void Model::tick()
{
//    LOG_INFO_WP("tick\n");

    if (mCounter >= 15) {
        //exitApp();
#if defined(SIMULATOR)        
        return;
#endif
    }

    mGSModel->process(0);
}

void Model::handleKeyEvent(uint8_t key)
{
    LOG_INFO("key = %c\n", static_cast<char>(key));

    // Hardwaare buttons
    if (Gui::Config::Button::L1 == key) {
        resetIdleTimer();
    }

    if (Gui::Config::Button::L2 == key) {
        resetIdleTimer();
    }

    if (Gui::Config::Button::R1 == key) {
        resetIdleTimer();
    }

    if (Gui::Config::Button::R2 == key) {
        resetIdleTimer();
    }
}

void Model::resetIdleTimer()
{
    mIdleTimer = Gui::Config::kScreenTimeoutSteps;
}

void Model::exitApp()
{
    LOG_INFO("exit from Utility2 GUI\n");
    mGSModel->post(G2SEvent::Stop{});
    mGSModel->setGUIHandler(nullptr, nullptr);
    mKernel.app.exit();
}

void Model::handleEvent(const S2GEvent::Counter& event)
{
    LOG_INFO("counter = %02ld.%ld\n", mCounter, event.value);
    modelListener->updateCounter(event.value);

    ++mCounter;
}

void Model::decIdleTimer()
{
    if (mIdleTimer > 0) {
        mIdleTimer--;
        if (mIdleTimer == 0) {
            modelListener->onIdleTimeout();
        }
    }
}

// IUserApp implementation
void Model::onCreate()
{
    LOG_INFO("called\n");
}

void Model::onStart()
{
    LOG_INFO("called\n");

    mGSModel->setGUIHandler(&mKernel, this);
    mGSModel->post(G2SEvent::Run {});
}

void Model::onResume()
{
    LOG_INFO("called\n");
}

void Model::onFrame()
{
    //LOG_INFO("called\n");
}

void Model::onPause()
{
    LOG_INFO("called\n");
}

void Model::onStop()
{
    LOG_INFO("called\n");

    mGSModel->post(G2SEvent::Stop {});
    mGSModel->setGUIHandler(nullptr, nullptr);
}

void Model::onDestroy()
{
    LOG_INFO("called\n");
}
