#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <gui/common/FrontendApplication.hpp>
#include "KernelManager.hpp"

#define TAG                 "Model"
#define LOG_MODULE_PRX      TAG"::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "Logger.h"

#if defined(SIMULATOR)
    #include "touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp"
    #include "Windows.h"
    #include <chrono>
    #include <ctime>
#endif

Model::Model()
    : mKernel(KernelManager::GetInstance().getKernel())
    , modelListener(0)
    , mGSModel(std::static_pointer_cast<IGUIModel>(mKernel->gctrl.getContext()))
    , mCounter(0)
{
    mKernel->app.registerApp(this);
    mGSModel->setGUIHandler(mKernel, this);
    mKernel->app.initialized();

    LOG_INFO("GUI [Utility #2] is initialized\n");

#if defined(SIMULATOR)
    LOG_DEBUG("Application is running through simulator! \n");

    LOG_DEBUG_WP("\n"
        "       Keys:                       \n"
        "       ----------------------------\n"
        "       1   L1,         !   Long L1 \n"
        "       2   L2,         @   Long L2 \n"
        "       3   R1,         #   Long R1 \n"
        "       4   R2,         $   Long R2 \n"
        "       q   L1+L2                   \n"
        "       e   R1+R2                   \n"
        "       a   L1+R1                   \n"
        "       s   L2+R2                   \n"
        "       z   L1+R2                   \n"
        "       w   R1+L2                   \n"
        "       +   Increase Batt level     \n"
        "       -   Decrease Batt level     \n"
        "       C   Toggle Batt charging    \n"
    );
#endif
}

void Model::tick()
{
//    LOG_INFO_WP("tick\n");

    if (mCounter >= 5) {
        mGSModel->sendToService(G2SEvent::Stop {});
        mGSModel->setGUIHandler(nullptr, nullptr);

        exitApp();
#if defined(SIMULATOR)        
        return;
#endif
    }

    mGSModel->checkS2GEvents(0);
}

void Model::handleKeyEvent(uint8_t key)
{
    LOG_INFO("key = %c\n", static_cast<char>(key));
}

FrontendApplication& Model::application()
{
    return *static_cast<FrontendApplication *>(touchgfx::Application::getInstance());
}

void Model::exitApp()
{
    LOG_INFO("exit from Utility2 GUI\n");
    mKernel->app.exit();
}

void Model::handleEvent(const S2GEvent::Counter& event)
{
    LOG_INFO("counter = %02ld.%ld\n", mCounter, event.value);
    modelListener->updateCounter(event.value);

    ++mCounter;
}

void Model::onCreate()
{
    LOG_INFO("called\n");
}

void Model::onStart()
{
    LOG_INFO("called\n");

    mGSModel->setGUIHandler(mKernel, this);
    mGSModel->sendToService(G2SEvent::Run {});
}

void Model::onResume()
{
    LOG_INFO("called\n");
}

void Model::onFrame()
{
//    LOG_INFO("called\n");
//    mKernel->app.waitForFrame();
}

void Model::onPause()
{
    LOG_INFO("called\n");
}

void Model::onStop()
{
    LOG_INFO("called\n");

    mGSModel->sendToService(G2SEvent::Stop {});
    mGSModel->setGUIHandler(nullptr, nullptr);
}

void Model::onDestroy()
{
    LOG_INFO("called\n");
}
