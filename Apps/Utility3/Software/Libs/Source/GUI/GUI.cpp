#include "GUI.hpp"
#include "SDK/Kernel/KernelProviderGUI.hpp"

#define LOG_MODULE_PRX      "GUI::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

GUI::GUI()
    : mKernel(SDK::KernelProviderGUI::GetInstance().getKernel())
    , mGSModel(std::static_pointer_cast<GSModelGUI>(mKernel.gctrl.getContext()))
    , mTerminate(false)
{
    mKernel.app.registerApp(this);
    mGSModel->setGUIHandler(&mKernel, this);
    mKernel.app.initialized();
}

GUI::~GUI()
{
    mGSModel->sendToService(G2SEvent::Stop {});
    mGSModel->setGUIHandler(nullptr, nullptr);
}

void GUI::run()
{
    mGSModel->sendToService(G2SEvent::Run {});

    while (!mTerminate) {
        mGSModel->checkS2GEvents();
    }
}

void GUI::handleEvent(const S2GEvent::Counter& event)
{
    LOG_INFO("counter = %ld\n", event.value);
}

void GUI::onCreate()
{
    LOG_INFO("onCreate()\n");
}

void GUI::onStart()
{
    LOG_INFO("onStart()\n");
}

void GUI::onResume()
{
    LOG_INFO("onResume()\n");
}

void GUI::onPause()
{
    LOG_INFO("onPause()\n");
}

void GUI::onStop()
{
    LOG_INFO("onStop()\n");
}

void GUI::onDestroy()
{
    LOG_INFO("onDestroy()\n");
    mTerminate = true;
}
