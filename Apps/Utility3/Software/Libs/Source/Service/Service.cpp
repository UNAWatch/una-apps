#include "Service.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"

#define LOG_MODULE_PRX      "Service::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

Service::Service()
    : mKernel(SDK::KernelProviderService::GetInstance().getKernel())
    , mGSModel(std::make_shared<GSModelService>(*this))
    , mTerminate(false)
    , mCounter(0)
    , mGUIStarted(false)
    , mSensorStepCounter(SDK::Sensor::Type::STEP_COUNTER, this)
    , mSensorStepDetector(SDK::Sensor::Type::STEP_DETECTOR, this)
    , mSensorMotionDetect(SDK::Sensor::Type::MOTION_DETECT, this)
    , mSensorActivityRecognition(SDK::Sensor::Type::ACTIVITY_RECOGNITION, this)
    , mAltimeter(SDK::Sensor::Type::ALTIMETER, this)
    , mSensorFloorCounter(SDK::Sensor::Type::FLOOR_COUNTER, this)
    , mGPS(SDK::Sensor::Type::GPS, this)
{
    mKernel.app.registerApp(this);
    mKernel.sctrl.setContext(mGSModel);
    mKernel.app.initialized();
}

void Service::testSesnors()
{
    mSensorStepCounter.connect();
    mSensorStepDetector.connect();
    mSensorMotionDetect.connect();
    mSensorActivityRecognition.connect();
    mAltimeter.connect(2000, 4000);
    mSensorFloorCounter.connect();
    mGPS.connect();

    SDK::SwTimer timer(SDK::SwTimer::minutes(2) + SDK::SwTimer::seconds(20));
    while (!mTerminate && !timer.expired()) {
        mGSModel->checkG2SEvents();
    }

    mSensorStepCounter.disconnect();
    mSensorStepDetector.disconnect();
    mSensorMotionDetect.disconnect();
    mSensorActivityRecognition.disconnect();
    mAltimeter.disconnect();
    mSensorFloorCounter.disconnect();
    mGPS.disconnect();
}

void Service::run()
{
    SDK::SwTimer timer(SDK::SwTimer::minutes(2) + SDK::SwTimer::seconds(30));
    bool startSensors = true;

    while (!mTerminate) {
        mGSModel->checkG2SEvents();

        mCounter += 1;

        if (mGUIStarted) {
            S2GEvent::Counter counter = {
                .value = mCounter
            };

            mGSModel->sendToGUI(counter);
        }

        if (startSensors) {
            startSensors = false;
            testSesnors();
        }

        if (timer.check()) {
            startSensors = true;
        }
    }
}

void Service::sdlNewData(const SDK::Interface::ISensorDriver*             sensor,
                         const std::vector<SDK::Interface::ISensorData*>& data,
                         bool                                             first)
{
    const SDK::Interface::ISensorData& sample = *data[0];

    if (sensor->getType() == SDK::Sensor::Type::STEP_COUNTER) {
        SDK::SensorDataParser::StepCounter p(data[0]);
        if (p.isDataValid()) {
            LOG_INFO("step counts : %8ld - %ld\n", p.getTimestamp(), p.getStepCount());
        }
    } else if (sensor->getType() == SDK::Sensor::Type::STEP_DETECTOR) {
        SDK::SensorDataParser::StepDetector p(data[0]);
        if (p.isDataValid()) {
            LOG_INFO("step detect : %8ld\n", p.getTimestamp());
        }
    } else if (sensor->getType() == SDK::Sensor::Type::MOTION_DETECT) {
        SDK::SensorDataParser::MotionDetect p(data[0]);
        if (p.isDataValid()) {
            switch (p.getID()) {
                case SDK::SensorDataParser::MotionDetect::Motion::NO_MOTION:  LOG_INFO("motion      : %8ld - no motion\n",  p.getTimestamp()); break;
                case SDK::SensorDataParser::MotionDetect::Motion::MOTION:     LOG_INFO("motion      : %8ld - motion\n",     p.getTimestamp()); break;
                case SDK::SensorDataParser::MotionDetect::Motion::SIG_MOTION: LOG_INFO("motion      : %8ld - sig motion\n", p.getTimestamp()); break;
                default:                                                      LOG_INFO("motion      : ERROR\n");
            }
        }
    } else if (sensor->getType() == SDK::Sensor::Type::ACTIVITY_RECOGNITION) {
        SDK::SensorDataParser::ActivityRecognition p(data[0]);
        if (p.isDataValid()) {
            switch (p.getID()) {
                case SDK::SensorDataParser::ActivityRecognition::Activity::STILL:   LOG_INFO("activity    : %8ld - still\n",   p.getTimestamp()); break;
                case SDK::SensorDataParser::ActivityRecognition::Activity::WALKING: LOG_INFO("activity    : %8ld - walking\n", p.getTimestamp()); break;
                case SDK::SensorDataParser::ActivityRecognition::Activity::RUNNING: LOG_INFO("activity    : %8ld - running\n", p.getTimestamp()); break;
                case SDK::SensorDataParser::ActivityRecognition::Activity::UNKNOWN: LOG_INFO("activity    : %8ld - unknown\n", p.getTimestamp()); break;
                default:                                                            LOG_INFO("activity    : ERROR\n");
            }
        }
    } else if (sensor->getType() == SDK::Sensor::Type::ALTIMETER) {
        for (auto item : data) {
            SDK::SensorDataParser::Altimeter p(item);
            if (p.isDataValid()) {
                LOG_INFO("altimeter   : %8ld - %.2f\n", p.getTimestamp(), p.getAltitude());
            }
        }
    } else if (sensor->getType() == SDK::Sensor::Type::FLOOR_COUNTER) {
        SDK::SensorDataParser::FloorCounter p(data[0]);
        if (p.isDataValid()) {
            LOG_INFO("up/down     : %8ld - %d / %d\n", p.getTimestamp(), p.getFloorsUp(), p.getFloorsDown());
        }
    } else if (sensor->getType() == SDK::Sensor::Type::GPS) {
        SDK::SensorDataParser::GPS p(sample);
        if (p.isDataValid()) {
            LOG_INFO("mask  = %ld\n",  p.getMask());
            LOG_INFO("time  = %ld\n",  p.getTime());
            LOG_INFO("lat   = %f\n",   p.getLatitude());
            LOG_INFO("lon   = %f\n",   p.getLongitude());
            LOG_INFO("alt   = %f\n",   p.getAltitude());
            LOG_INFO("speed = %.1f\n", p.getSpeed());
        }
    }
}

void Service::handleEvent(const G2SEvent::Run& event)
{
    (void) event;

    LOG_INFO("G2SEvent::Run\n");

    mGUIStarted = true;
}

void Service::handleEvent(const G2SEvent::Stop& event)
{
    (void) event;

    LOG_INFO("G2SEvent::Stop\n");

    mGUIStarted = false;
}

void Service::onDestroy()
{
    mTerminate = true;
}
