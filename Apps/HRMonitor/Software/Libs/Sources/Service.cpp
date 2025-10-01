#include "Service.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRate.hpp"

#define LOG_MODULE_PRX      "Service::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

Service::Service(const IKernel& kernel)
        : mKernel(kernel)
        , mGSModel(std::make_shared<GSModelService>(kernel, *this))
        , mTerminate(false)
        , mGUIStarted(false)
        , mHRSensor(nullptr)
{
    mKernel.app.registerApp(this);
    mKernel.sctrl.setContext(mGSModel);
}

void Service::run()
{
    mKernel.app.initialized();

    LOG_INFO("started\n");

    mHRSensor = mKernel.sensorManager.getDefaultSensor(SDK::Sensor::Type::HEART_RATE);
    if (mHRSensor) {
        mHRSensor->connect(this, &mKernel.app, 1000, 2000);
    }

    while (!mTerminate) {
        mGSModel->checkG2SEvents(1000);
    }

    if (mHRSensor) {
        mHRSensor->disconnect(this);
    }

    LOG_INFO("stopped\n");
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

void Service::onStop()
{
    mTerminate = true;
}

void Service::sdlNewData(const SDK::Interface::ISensorDriver* sensor, const std::vector<SDK::Interface::ISensorData*>& data, bool first)
{
    if (sensor == mHRSensor) {
        if (mGUIStarted) {
            SDK::SensorDataParser::HeartRate hrp { data[0]};
            if (hrp.isDataValid()) {
                float hr = hrp.getBpm();
                float tl = hrp.getTrustLevel();

                mGSModel->sendToGUI(S2GEvent::HeartRate{hr, tl});
            }
        }
    }
}
