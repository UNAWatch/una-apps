#include "Service.hpp"

#include <cstdio>

#define LOG_MODULE_PRX      LOG_TAG"Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "Logger.h"

Service::Service(const IKernel& kernel)
        : mKernel(kernel)
        , mGSModel(std::make_shared<GSModel>(kernel, *this))
        , mTerminate(false)
        , mGUIStarted(false)
{
    mKernel.app.registerApp(this);
    mKernel.sctrl.setContext(mGSModel);
}

void Service::run()
{
    mKernel.app.initialized();

    while (!mTerminate) {
        mGSModel->checkG2SEvents(1000);


        if (mGUIStarted) {

        }
    }

}

void Service::handleEvent(const G2SEvent::Run& event)
{
    (void) event;
    LOG_DEBUG("G2SEvent::Run\n");
    mGUIStarted = true;
}

void Service::handleEvent(const G2SEvent::Stop& event)
{
    (void) event;
    LOG_DEBUG("G2SEvent::Stop\n");
    mGUIStarted = false;
}

void Service::handleEvent(const G2SEvent::InternalRefresh& event)
{
    // do nothing
}

void Service::onCreate()
{
    LOG_DEBUG("called\n");
}

void Service::onStart()
{
    LOG_DEBUG("called\n");
}

void Service::onResume()
{
    LOG_DEBUG("called\n");
}

void Service::onStop()
{
    LOG_DEBUG("called\n");
    mTerminate = true;
    refreshService();
}

void Service::onPause()
{
    LOG_DEBUG("called\n");
}

void Service::onDestroy()
{
    LOG_DEBUG("called\n");
}

void Service::refreshService()
{
    mGSModel->sendToService(G2SEvent::InternalRefresh{});
}
