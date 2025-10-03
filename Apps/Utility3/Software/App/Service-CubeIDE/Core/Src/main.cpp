/**
 ******************************************************************************
 * @file    main.сpp
 * @date    05-07-2024
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Main program body.
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <cstdint>
#include <cstring>
#include <cassert>
#include <math.h>
#include <stdio.h>
#include <memory>

#include "SDK/GSModel/GSModelHelper.hpp"
#include "SDK/Interfaces/ISensorDriver.hpp"
#include "SDK/SensorLayer/SensorDriverConnection.hpp"
#include "SDK/Interfaces/ISensorDataListener.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserStepCounter.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserStepDetector.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserMotionDetect.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserActivityRecognition.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserAltimeter.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserFloorCounter.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserGPS.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRate.hpp"
#include "SDK/SwTimer/SwTimer.hpp"

#define LOG_MODULE_PRX      "main::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

static const char* mName = "Service::Utility#3";

class Service : public IServiceModelHandler,
                public SDK::Interface::IUserApp::Callback,
                public SDK::Interface::ISensorDataListener
{
public:
    Service(const IKernel& kernel)
        : mKernel(kernel)
        , mGSModel(std::make_shared<GSModelService>(mKernel, *this))
        , mTerminate(false)
        , mCounter(0)
        , mGUIStarted(false)
        , mSensorStepCounter(kernel.sensorManager.getDefaultSensor(SDK::Sensor::Type::STEP_COUNTER))
        , mSensorStepDetector(kernel.sensorManager.getDefaultSensor(SDK::Sensor::Type::STEP_DETECTOR))
        , mSensorMotionDetect(kernel.sensorManager.getDefaultSensor(SDK::Sensor::Type::MOTION_DETECT))
        , mSensorActivityRecognition(kernel.sensorManager.getDefaultSensor(SDK::Sensor::Type::ACTIVITY_RECOGNITION))
        , mAltimeter(SDK::Sensor::Type::ALTIMETER, this)
        , mSensorFloorCounter(kernel.sensorManager.getDefaultSensor(SDK::Sensor::Type::FLOOR_COUNTER))
        , mGPS(SDK::Sensor::Type::GPS, this)
        , mHeartRate(SDK::Sensor::Type::HEART_RATE, this)
    {
        mKernel.app.registerApp(this);
        mKernel.sctrl.setContext(mGSModel);
        mKernel.app.initialized();
    }

    virtual ~Service() = default;

    void testSesnors()
    {
//        mSensorStepCounter->connect(this, &mKernel.app);
//        mSensorStepDetector->connect(this, &mKernel.app);
//        mSensorMotionDetect->connect(this, &mKernel.app);
//        mSensorActivityRecognition->connect(this, &mKernel.app);
        mAltimeter.connect(2000, 4000);
//        mSensorFloorCounter->connect(this, &mKernel.app);
//        mGPS.connect();
        mHeartRate.connect();

        SDK::SwTimer timer(SDK::SwTimer::minutes(2) + SDK::SwTimer::seconds(20));
        while (!mTerminate && !timer.expired()) {
            mGSModel->checkG2SEvents();
        }

//        mSensorStepCounter->disconnect(this);
//        mSensorStepDetector->disconnect(this);
//        mSensorMotionDetect->disconnect(this);
//        mSensorActivityRecognition->disconnect(this);
        mAltimeter.disconnect();
//        mSensorFloorCounter->disconnect(this);
//        mGPS.disconnect();
        mHeartRate.disconnect();
    }

    void run()
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

    void sdlNewData(const SDK::Interface::ISensorDriver*             sensor,
                    const std::vector<SDK::Interface::ISensorData*>& data,
                    bool                                             first) override
    {
        const  SDK::Interface::ISensorData& sample = *data[0];

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
        } else if (sensor->getType() == SDK::Sensor::Type::HEART_RATE) {
            SDK::SensorDataParser::HeartRate p(sample);
            if (p.isDataValid()) {
                LOG_INFO("bpm  = %ld\n",  p.getBpm());
                LOG_INFO("time = %ld\n",  p.getTimestamp());
            }
        }
    }

    void handleEvent(const G2SEvent::Run& event)
    {
        (void) event;

        printf("%s::G2SEvent::Run\n", mName);

        mGUIStarted = true;
    }

    void handleEvent(const G2SEvent::Stop& event)
    {
        (void) event;

        printf("%s::G2SEvent::Stop\n", mName);

        mGUIStarted = false;
    }

private:
    void onDestroy() override
    {
        mTerminate = true;
    }

    const IKernel&                  mKernel;

    std::shared_ptr<GSModelService> mGSModel;
    bool                            mTerminate;
    uint32_t                        mCounter;
    bool                            mGUIStarted;

    SDK::Interface::ISensorDriver* mSensorStepCounter;
    SDK::Interface::ISensorDriver* mSensorStepDetector;
    SDK::Interface::ISensorDriver* mSensorMotionDetect;
    SDK::Interface::ISensorDriver* mSensorActivityRecognition;
    SDK::Sensors::DriverConnection mAltimeter;
    SDK::Interface::ISensorDriver* mSensorFloorCounter;
    SDK::Sensors::DriverConnection mGPS;
    SDK::Sensors::DriverConnection mHeartRate;
};

extern const IKernel* kernel;

static uint32_t LoggerGetTicks()
{
    return kernel->app.getTimeMs();
}

static void LoggerPrint(const char* str)
{
    kernel->app.log(str);
}

/**
 * @brief   The application entry point.
 * @retval  int
 */
int main()
{
    Logger_init(LoggerPrint);
    Logger_setTimeFunc(LoggerGetTicks);

    LOG_INFO("%s is started\n", mName);

    Service service(*kernel);
    service.run();

    return 0;
}

