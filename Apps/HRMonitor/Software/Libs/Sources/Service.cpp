#include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRate.hpp"

#include "Service.hpp"

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

Service::Service()
    : mKernel(SDK::KernelProviderService::GetInstance().getKernel())
    , mGSModel(*this)
    , mTerminate(false)
    , mGUIStarted(false)
    , mHRSensor(SDK::Sensor::Type::HEART_RATE, this)
    , mActivityWriter(mKernel, "Activity")
{}

void Service::run()
{
    LOG_INFO("thread started\n");

    mHRSensor.connect(1000, 2000);

    ActivityWriter::AppInfo info{};
    info.timestamp = std::time(nullptr);
    info.appVersion = ParseVersion(BUILD_VERSION);
    info.devID = DEV_ID;
    info.appID = APP_ID;
    mActivityWriter.start(info);

    time_t startTime = time(nullptr);
    time_t utcTimestamp = 0;

    float hrAvgSum = 0;
    uint32_t hrAvgCount = 0;
    float hrMax = 0;

    uint32_t startTimeMs = mKernel.system.getTimeMs();

    while (!mTerminate) {

        mGSModel.process(1000);

        if (mGUIStarted) {
            // Save record to the FIT file
            time_t utc = time(nullptr);
            if (utcTimestamp != utc) {
                utcTimestamp = utc;

                ActivityWriter::RecordData fitRecord {};
                fitRecord.timestamp = utc;
                fitRecord.heartRate = static_cast<uint8_t>(mHR);
                fitRecord.trustLevel = static_cast<uint8_t>(mHRTL);
                mActivityWriter.addRecord(fitRecord);

                if (mHR > 1) {
                    hrAvgSum += mHR;
                    hrAvgCount ++;
                    hrMax = std::max(hrMax, mHR);
                }
            }
        } else {
            // Just wait some time to see if GUI starts
            if (mKernel.system.getTimeMs() - startTimeMs > 5000) {
                break;
            }
            mKernel.system.delay(100);
        }
    }

    time_t utc = time(nullptr);

    // Save lap to the FIT file
    ActivityWriter::LapData fitLap {};
    fitLap.timeStart = utc - startTime;
    fitLap.duration = utc - startTime;
    fitLap.elapsed = utc - startTime;
    fitLap.hrAvg = static_cast<uint8_t>(hrAvgSum / hrAvgCount);
    fitLap.hrMax = static_cast<uint8_t>(hrMax);
    mActivityWriter.addLap(fitLap);

    // Create FIT file

    ActivityWriter::TrackData fitTrack{};
    fitTrack.timeStart = utc;
    fitTrack.duration = utc - startTime;
    fitTrack.elapsed = utc - startTime;
    fitTrack.hrAvg = static_cast<uint8_t>(hrAvgSum / hrAvgCount);
    fitTrack.hrMax = static_cast<uint8_t>(hrMax);

    mActivityWriter.stop(fitTrack);

    mHRSensor.disconnect();

    LOG_INFO("thread stopped\n");
}

void Service::onStart()
{
    LOG_INFO("called\n");
}

void Service::onStop()
{
    LOG_INFO("called\n");
    mTerminate = true;
    mGSModel.abortProcessWait();
}

void Service::onStartGUI()
{
    LOG_INFO("GUI started\n");
    mGUIStarted = true;
    mGSModel.post(S2GEvent::HeartRate{0, 0});
}

void Service::onStopGUI()
{
    LOG_INFO("GUI stopped\n");
    mGUIStarted = false;
    mGSModel.abortProcessWait();
}

void Service::onSdlNewData(const SDK::Interface::ISensorDriver* sensor, const std::vector<SDK::Interface::ISensorData*>& data, bool first)
{
    if (sensor->getType() == SDK::Sensor::Type::HEART_RATE) {
        if (mGUIStarted) {
            SDK::SensorDataParser::HeartRate hrp { data[0]};
            if (hrp.isDataValid()) {
                mHR = hrp.getBpm();
                mHRTL = hrp.getTrustLevel();

                mGSModel.post(S2GEvent::HeartRate{mHR, mHRTL});
            }
        }
    }
}

uint32_t Service::ParseVersion(const char* str)
{
    if (str == nullptr) {
        return 0;
    }

    typedef union {
        struct {
            uint8_t patch;
            uint8_t minor;
            uint8_t major;
        };
        uint32_t u32;
    } FirmwareVersion_t;

    FirmwareVersion_t v{};

    int major, minor, patch;

    if (sscanf(str, "%d.%d.%d", &major, &minor, &patch) == 3) {
        v.major = static_cast<uint8_t>(major);
        v.minor = static_cast<uint8_t>(minor);
        v.patch = static_cast<uint8_t>(patch);
        return v.u32;
    }

    return 0;
}
