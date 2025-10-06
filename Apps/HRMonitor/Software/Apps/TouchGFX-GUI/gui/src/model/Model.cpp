#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <gui/common/FrontendApplication.hpp>

#include "SDK/Kernel/KernelProviderGUI.hpp"


#define LOG_MODULE_PRX      "Model"
#define LOG_MODULE_LEVEL    LOG_LEVEL_INFO
#include "SDK/UnaLogger/Logger.h"

#if defined(SIMULATOR)
    #include "touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp"
    #include "Windows.h"
    #include <chrono>
    #include <ctime>
#endif

Model::Model()
    : modelListener(0)
    , mKernel(SDK::KernelProviderGUI::GetInstance().getKernel())
    , mGSModel(std::static_pointer_cast<IGUIModel>(mKernel.gctrl.getContext()))
{
    mKernel.app.registerApp(this);
    mGSModel->setGUIHandler(&mKernel, this);

#if defined(SIMULATOR)
    LOG_INFO("Application is running through simulator! \n");

    std::string fileStoreDir = SDK::Simulator::KernelHolder::Get().getFsPath();
    LOG_INFO("Path to files created by app:\n"
        "       [% s]\n", fileStoreDir.c_str());

    LOG_INFO("\n"
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
    mGSModel->process(0);
}

void Model::exitApp()
{
    LOG_INFO("called\n");
    mGSModel->setGUIHandler(nullptr, nullptr);
    mKernel.app.exit();
}

void Model::handleEvent(const S2GEvent::HeartRate& event)
{
    //LOG_INFO("hr %.1f, tl %.1f\n", event.heartRate, event.trustLevel);
    modelListener->updateHR(event.heartRate, event.trustLevel);
}

// IUserApp implementation
void Model::onStart()
{
    LOG_INFO("called\n");
}

void Model::onStop()
{
    LOG_INFO("called\n");
    mGSModel->setGUIHandler(nullptr, nullptr);
}
