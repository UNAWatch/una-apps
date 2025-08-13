#include "Service.hpp"

#include <stdio.h>

#define TAG                 "ServiceModule"
#define LOG_MODULE_PRX      TAG"::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "Logger.h"

Service::Service(const IKernel& kernel)
        : mKernel(kernel)
        , mGSModel(std::make_shared<GSModel>(kernel, *this))
        , mTerminate(false)
        , mCounter(0)
        , mGUIStarted(false)
        , mDS(nullptr)
        , mBMETemp(nullptr)
        , mBMEPressure(nullptr)
        , mAltimeter(nullptr)
{
    mKernel.app.registerApp(this);
    mKernel.sctrl.setContext(mGSModel);
}

void Service::run()
{
    mKernel.app.initialized();

    mKernel.app.enableMusicControl(false);
    mKernel.app.enablePhoneNotification(false);
    mKernel.app.enableUsbCharging(false);
    mKernel.app.enableMusicControl(true);
    mKernel.app.enablePhoneNotification(true);
    mKernel.app.enableUsbCharging(true);

    uint8_t stage = 0;

    mDS = mKernel.sensorManager.getDefaultSensor(Sensor::Type::SENSOR_TYPE_AMBIENT_TEMPERATURE);
    mDS->connect(this, &mKernel.app, 1000, 1000);

    std::vector<Interface::ISensorDriver*> tempList = mKernel.sensorManager.getSensorList(Sensor::Type::SENSOR_TYPE_AMBIENT_TEMPERATURE);
    mBMETemp = tempList[1];
    mBMETemp->connect(this, &mKernel.app, 1000, 1000);

    mBMEPressure = mKernel.sensorManager.getDefaultSensor(Sensor::Type::SENSOR_TYPE_PRESSURE);
    mBMEPressure->connect(this, &mKernel.app, 1000, 1000);

    mAltimeter = mKernel.sensorManager.getDefaultSensor(Sensor::Type::SENSOR_TYPE_ALTIMETER);
    mAltimeter->connect(this, &mKernel.app, 1000, 1000);

    while (!mTerminate) {
        mGSModel->checkG2SEvents(1000);

        LOG_INFO("test\n");

        if (stage == 0) {
            mKernel.backlight.on(1000);
            mKernel.backlight.off();
        } else if (stage == 1) {
            mKernel.vibro.play();
            mKernel.vibro.stop();
        } else {
            mKernel.buzzer.play();
            mKernel.buzzer.stop();
        }

        if (++stage > 2) {
            stage = 0;
        }

        mCounter += 10;

        if (mGUIStarted) {
            S2GEvent::Counter counter = {
                    .value = mCounter
            };

            mGSModel->sendToGUI(counter);
        }
    }

    mDS->disconnect(this);
    mBMETemp->disconnect(this);
    mBMEPressure->disconnect(this);
    mAltimeter->disconnect(this);
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

void Service::onNewSensorData(const Interface::ISensorDriver*             sensor,
                              const std::vector<Interface::ISensorData*>& data,
                              bool                                        first)
{
    if (sensor == mDS) {
        LOG_INFO("DS18B20   = %.2f\n", data[0]->getValue(0));
    } else if (sensor == mBMETemp) {
        LOG_INFO("BME.T     = %.2f\n", data[0]->getValue(0));
    } else if (sensor == mBMEPressure) {
        LOG_INFO("BME.P     = %.2f\n", data[0]->getValue(0));
    } else if (sensor == mAltimeter) {
        LOG_INFO("Altimeter = %.2f\n", data[0]->getValue(0));
    } else {
        LOG_ERROR("unknown sensor\n");
    }
}
