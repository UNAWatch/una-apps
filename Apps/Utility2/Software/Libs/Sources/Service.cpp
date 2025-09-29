#include "Service.hpp"

#include <stdio.h>

#define LOG_MODULE_PRX      "Service::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

Service::Service()
        : mKernel(SDK::KernelProviderService::GetInstance().getKernel())
        , mGSModel(std::make_shared<GSModelService>(*this))
        , mTerminate(false)
        , mCounter(0)
        , mGUIStarted(false)
        , mBMETemp(SDK::Sensor::Type::AMBIENT_TEMPERATURE, this)
        , mBMEPressure(SDK::Sensor::Type::PRESSURE, this)
        , mAltimeter(SDK::Sensor::Type::ALTIMETER, this)
{
    mKernel.app.registerApp(this);
    mKernel.sctrl.setContext(mGSModel);
}

void Service::run()
{
    mKernel.app.initialized();

    mBMETemp.connect();
    mBMEPressure.connect();
    mAltimeter.connect();

    while (!mTerminate) {
        mGSModel->checkG2SEvents(1000);

        mCounter += 10;

        if (mGUIStarted) {
            S2GEvent::Counter counter = { mCounter };

            mGSModel->sendToGUI(counter);
        }
    }

    mBMETemp.disconnect();
    mBMEPressure.disconnect();
    mAltimeter.disconnect();
}

void Service::handleEvent(const G2SEvent::Run& event)
{
    (void) event;

    mGUIStarted = true;
}

void Service::handleEvent(const G2SEvent::Stop& event)
{
    (void) event;

    mGUIStarted = false;
}

void Service::onCreate()
{
    LOG_INFO("called\n");
}

void Service::onStart()
{
    LOG_INFO("called\n");
}

void Service::onResume()
{
    LOG_INFO("called\n");
}

void Service::onStop()
{
    LOG_INFO("called\n");
    mTerminate = true;
}

void Service::onPause()
{
    LOG_INFO("called\n");
}

void Service::onDestroy()
{
    LOG_INFO("called\n");
}

void Service::sdlNewData(const SDK::Interface::ISensorDriver*             sensor,
                         const std::vector<SDK::Interface::ISensorData*>& data,
                         bool                                             first)
{
    if (sensor->getType() == SDK::Sensor::Type::AMBIENT_TEMPERATURE) {
        LOG_INFO("BME.T     = %.2f\n", data[0]->getAsFloat(0));
    } else if (sensor->getType() == SDK::Sensor::Type::PRESSURE) {
        LOG_INFO("BME.P     = %.2f\n", data[0]->getAsFloat(0));
    } else if (sensor->getType() == SDK::Sensor::Type::ALTIMETER) {
        LOG_INFO("Altimeter = %.2f (%d)\n", data[0]->getAsFloat(0), data.size());
    } else {
        LOG_ERROR("unknown sensor\n");
    }
}
