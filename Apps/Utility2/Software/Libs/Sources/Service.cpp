#include "Service.hpp"

#include <stdio.h>

#define TAG                 "Service2"
#define LOG_MODULE_PRX      TAG"::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "Logger.h"

Service::Service(const IKernel& kernel)
        : mKernel(kernel)
        , mGSModel(std::make_shared<GSModelService>(kernel, *this))
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

//    mKernel.app.enableMusicControl(false);
//    mKernel.app.enablePhoneNotification(false);
//    mKernel.app.enableUsbCharging(false);
//    mKernel.app.enableMusicControl(true);
//    mKernel.app.enablePhoneNotification(true);
//    mKernel.app.enableUsbCharging(true);

    uint8_t stage = 0;

    mAltimeter = mKernel.sensorManager.getDefaultSensor(SDK::Sensor::Type::ALTIMETER);
    if (mAltimeter) {
        mAltimeter->connect(this, &mKernel.app, 1000, 2000);
    }

    while (!mTerminate) {
        mGSModel->checkG2SEvents(1000);

//        if (stage == 0) {
//            mKernel.backlight.on(1000);
//            mKernel.backlight.off();
//        } else if (stage == 1) {
////            mKernel.vibro.play();
////            mKernel.vibro.stop();
//        } else {
//            mKernel.buzzer.play();
//            mKernel.buzzer.stop();
//        }
//
//        if (++stage > 2) {
//            stage = 0;
//        }

        mCounter += 10;

        if (mGUIStarted) {
            S2GEvent::Counter counter = { mCounter };

            mGSModel->sendToGUI(counter);
        }
    }

    if (mAltimeter) {
        mAltimeter->disconnect(this);
    }
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

void Service::onNewSensorData(const SDK::Interface::ISensorDriver*             sensor,
                              const std::vector<SDK::Interface::ISensorData*>& data,
                              bool                                             first)
{
    if (sensor == mDS) {
        LOG_INFO("DS18B20   = %.2f\n", data[0]->getAsFloat(0));
    } else if (sensor == mBMETemp) {
        LOG_INFO("BME.T     = %.2f\n", data[0]->getAsFloat(0));
    } else if (sensor == mBMEPressure) {
        LOG_INFO("BME.P     = %.2f\n", data[0]->getAsFloat(0));
    } else if (sensor == mAltimeter) {
        LOG_INFO("Altimeter = %.2f (%d)\n", data[0]->getAsFloat(0), data.size());
    } else {
        LOG_ERROR("unknown sensor\n");
    }
}
