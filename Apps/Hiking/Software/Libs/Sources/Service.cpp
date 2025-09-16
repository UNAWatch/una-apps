#include "Service.hpp"

#define LOG_MODULE_PRX      "Service::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include <ctime>
#include <cmath>
#include <memory>
#include <cstring>

#include "Settings.hpp"
#include "ActivitySummary.hpp"
#include "TrackInfo.hpp"

#include "SDK/SensorLayer/DataParsers/SensorDataParserGPS.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserAltimeter.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRate.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserStepCounter.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserFloorCounter.hpp"


Service::Service(const IKernel& kernel)
        : mKernel(kernel)
        , mGSModel(std::make_shared<GSModelService>(mKernel, *this))
        , mTerminate(false)
        , mGUIStarted(false)
        , mSettings{}
        , mSettingsSerializer(mKernel, "settings.json")
        , mSummary{}
        , mActivitySummarySerializer(mKernel, "Activity/summary.json")
        , mActivityWriter(mKernel, "Activity")
{
    mKernel.app.registerApp(this);
    mKernel.sctrl.setContext(mGSModel);
}

void Service::run()
{
    mKernel.app.initialized();

    // Get settings
    if (!mSettingsSerializer.load(mSettings)) {
        LOG_ERROR("Failed to load settings\n");
    }

    // Get summary
    if (!mActivitySummarySerializer.load(mSummary)) {
        LOG_ERROR("Failed to load activity summary\n");
    }


    mGpsSensor = mKernel.sensorManager.getDefaultSensor(SDK::Sensor::Type::GPS);
    mHrSensor = mKernel.sensorManager.getDefaultSensor(SDK::Sensor::Type::HEART_RATE);
    mStepCounterSensor = mKernel.sensorManager.getDefaultSensor(SDK::Sensor::Type::STEP_COUNTER);
    mFloorCounterSensor = mKernel.sensorManager.getDefaultSensor(SDK::Sensor::Type::FLOOR_COUNTER);
    mAltimeterSensor = mKernel.sensorManager.getDefaultSensor(SDK::Sensor::Type::ALTIMETER);

    // Subscribe to GPS to get fix
    if (mGpsSensor) {
        mGpsSensor->connect(this, &mKernel.app, skInitialSamplePeriod, skSampleLatency);
    }

    uint32_t startTime = mKernel.app.getTimeMs();

    while (!mTerminate) {
        mGSModel->checkG2SEvents(1000);

        if (mGUIStarted) {

            // Update time every second
            time_t utc = time(nullptr);
            std::tm tmNow{};
            #if WIN32
                localtime_s(&tmNow, &utc);
            #else
                localtime_r(&utc, &tmNow);
            #endif
            mGSModel->sendToGUI(S2GEvent::Time{ tmNow });

            // Update GPS fix

            if (mPreviousGpsFixState != mGps.fix) {
                mPreviousGpsFixState = mGps.fix;
                mGSModel->sendToGUI(S2GEvent::GpsFix{ mGps.fix });
            }

            if (mTrackState != Track::State::INACTIVE) {
                processTrack(utc);
            }

        } else {
            // Just wait some time to see if GUI starts
            if (mKernel.app.getTimeMs() - startTime > 5000) {
                break;
            }
        }
    }

    LOG_DEBUG("Exit service\n");

    // Unsubscribe from sensors
    if (mGpsSensor) {
        mGpsSensor->disconnect(this);
    }

    if (mHrSensor) {
        mHrSensor->disconnect(this);
    }

    if (mStepCounterSensor) {
        mStepCounterSensor->disconnect(this);
    }

    if (mAltimeterSensor) {
        mAltimeterSensor->disconnect(this);
    }

    if (mFloorCounterSensor) {
        mFloorCounterSensor->disconnect(this);
    }

    exit(0);
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

void Service::onNewSensorData(const SDK::Interface::ISensorDriver* sensor,
    const std::vector< SDK::Interface::ISensorData*>& data, bool first)
{
    // We are only interested in the last sample
    size_t sapleNum = data.size();
    if (sapleNum == 0) {
        return;
    }
    const  SDK::Interface::ISensorData& sample = *data[sapleNum -1];

    // [time, fix, speed, acc], 
    // [mask, time, lat, lon, alt, speed,  ...]
    if (sensor == mGpsSensor) {

        SDK::SensorDataParser::GPS gps {sample};
        if (gps.isDataValid()) {
            mGps.timestamp = sample.getTimestamp();
            mGps.fix = gps.isCoordinatesValid();

            if (mGps.fix) { // Do not change position if no fix
                gps.getCoordinates(mGps.latitude, mGps.longitude, mGps.altitude);
                //LOG_DEBUG("lat %f, lon %f\n", mGps.latitude, mGps.longitude);
            }
        }
    } else if (sensor == mStepCounterSensor) {
        SDK::SensorDataParser::StepCounter sc {sample};
        if (sc.isDataValid()) {
            if (mStepCounter.initialSteps == 0) {
                mStepCounter.initialSteps = sc.getStepCount();
            }
            mStepCounter.steps = sc.getStepCount();
            mStepCounter.timestamp = sample.getTimestamp();
            //LOG_DEBUG("steps %u\n", mStepCounter.steps);
        }
    } else if (sensor == mFloorCounterSensor) {
        SDK::SensorDataParser::FloorCounter fc {sample};
        if (fc.isDataValid()) {
            if (mFloorsCounter.initialFloors == 0) {
                mFloorsCounter.initialFloors = fc.getFloorCount();
            }
            mFloorsCounter.floors = fc.getFloorCount();
            mFloorsCounter.timestamp = sample.getTimestamp();
            //LOG_DEBUG("floors %u\n", mFloorsCounter.floors);
        }
    } else if (sensor == mAltimeterSensor) {
        SDK::SensorDataParser::Altimeter alt {sample};
        if (alt.isDataValid()) {
            if (std::abs(mAltimeter.initialAltitude) < 0.01) {
                mAltimeter.initialAltitude = alt.getAltitude();
            }
            mAltimeter.altitude = alt.getAltitude();
            mAltimeter.timestamp = sample.getTimestamp();
            //LOG_DEBUG("altitude %f\n", mAltimeter.altitude);
        }
    } else if (sensor == mHrSensor) {
        SDK::SensorDataParser::HeartRate hr {sample};
        if (hr.isDataValid()) {
            mHr.hr = static_cast<uint8_t>(hr.getBpm());
            mHr.timestamp = sample.getTimestamp();

            mHr.totalSum += mHr.hr;
            mHr.totalCnt++;
            //LOG_DEBUG("hr %u\n", mHr.hr);
        }
    }
}

void Service::handleEvent(const G2SEvent::GuiRun& event)
{
    (void)event;
    LOG_DEBUG("G2SEvent::GuiRun\n");
    mGUIStarted = true;
    sendInitialInfoToGui();
    refreshService();
}

void Service::handleEvent(const G2SEvent::GuiStop& event)
{
    (void)event;
    LOG_DEBUG("G2SEvent::GuiStop\n");
    mGUIStarted = false;
    refreshService();
}

void Service::handleEvent(const G2SEvent::TrackStart& event)
{
    LOG_DEBUG("G2SEvent::TrackStart\n");
    startTrack();
    refreshService();
}

void Service::handleEvent(const G2SEvent::TrackStop& event)
{
    LOG_DEBUG("G2SEvent::TrackStop\n");
    stopTrack(event.discard);
    refreshService();
}

void Service::handleEvent(const G2SEvent::SettingsSave& event)
{
    LOG_DEBUG("G2SEvent::SettingsSave\n");
    mSettings = event.settings;
    mSettingsSerializer.save(event.settings);
}

void Service::handleEvent(const G2SEvent::InternalRefresh& event)
{
    // do nothing
}




void Service::refreshService()
{
    mGSModel->sendToService(G2SEvent::InternalRefresh{});
}

void Service::sendInitialInfoToGui()
{
    // Settings
    mGSModel->sendToGUI(S2GEvent::SettingsUpd{ mSettings, mKernel.settings.isUnitsImperial(), mKernel.settings.getHrThresholds() });

    // Summary
    mGSModel->sendToGUI(S2GEvent::Summary{ std::make_shared<const ActivitySummary>(mSummary) });

    // Battery level
    uint8_t batteryLevel = static_cast<uint8_t>(mKernel.pwr.getBatteryLevel());
    mGSModel->sendToGUI(S2GEvent::Battery{ batteryLevel });
}

void Service::startTrack()
{
    // Reset data
    memset(&mTrackData, 0, sizeof(mTrackData));

    // Assume that GPS has fix
    mPrevGpsPoint.latitude = mGps.latitude;
    mPrevGpsPoint.longitude = mGps.longitude;

    // Configure TrackMapBuilder
    mTrackMapBuilder.reset();
    mTrackMapBuilder.setDistanceThreshold(mPrevGpsPoint, skMapDistanceThreshold);

    mStepCounter.initialSteps = 0;
    mFloorsCounter.initialFloors = 0;

    mAltimeter.initialAltitude = 0;
    mAltimeter.ascent = 0.0f;
    mAltimeter.descent = 0.0f;
    mAltimeter.lapAscent = 0.0f;
    mAltimeter.lapDescent = 0.0f;

    mHr.totalCnt = 0;
    mHr.totalSum = 0;
    mHr.lapCnt = 0;
    mHr.lapSum = 0;


    mTrackStartUTC = std::time(nullptr);
    mTrackProcessTimestamp = mTrackStartUTC;

    if (mStepCounterSensor) {
        mStepCounterSensor->connect(this, &mKernel.app, skInitialSamplePeriod, skSampleLatency);
    }

    if (mFloorCounterSensor) {
        mFloorCounterSensor->connect(this, &mKernel.app, skInitialSamplePeriod, skSampleLatency);
    }

    if (mAltimeterSensor) {
        mAltimeterSensor->connect(this, &mKernel.app, skInitialSamplePeriod, skSampleLatency);
    }

    if (mHrSensor) {
        mHrSensor->connect(this, &mKernel.app, skInitialSamplePeriod, skSampleLatency);
    }

    ActivityWriter::AppInfo info{};
    info.timestamp = std::time(nullptr);
    info.appVersion = ParseVersion(BUILD_VERSION);
    info.devID = DEV_ID;
    info.appID = APP_ID;
    mActivityWriter.start(info);

    mTrackState = Track::State::ACTIVE;

    mGSModel->sendToGUI(S2GEvent::TrackStateUpd{ mTrackState });
}

void Service::processTrack(std::time_t utc)
{
    std::time_t trackTime = static_cast<std::time_t>(std::difftime(utc, mTrackStartUTC));
    std::time_t trackTimeDiff = static_cast<std::time_t>(std::difftime(trackTime, mTrackData.totalTime));

    mTrackData.totalTime = trackTime;
    mTrackData.lapTime += trackTimeDiff;

    std::time_t timeDiff = static_cast<std::time_t>(std::difftime(utc, mTrackProcessTimestamp));
    if (timeDiff < (skSamplePeriod / 1000)) { // in seconds
        if (trackTimeDiff > 0) {
            mGSModel->sendToGUI(S2GEvent::TrackDataUpd{ mTrackData });
        }
        return;
    }


    // Process sensors
    std::time_t processTrackTimeDiff = static_cast<std::time_t>(std::difftime(utc, mTrackProcessTimestamp));
    mTrackProcessTimestamp = utc;

    // HR
    mTrackData.HR = mHr.hr;
    if (mHr.totalSum > 1 && mHr.totalCnt > 1) {
        mTrackData.avgHR = mHr.totalSum / mHr.totalCnt;
    } else {
        mTrackData.avgHR = mTrackData.HR;
    }
    mTrackData.maxHR = std::max(mTrackData.maxHR, mTrackData.HR);
    
    if (mHr.lapSum > 1 && mHr.lapCnt > 1) {
        mTrackData.avgLapHR = mHr.lapSum / mHr.lapCnt;
    } else {
        mTrackData.avgLapHR = mTrackData.HR;
    }
    mTrackData.maxLapHR = std::max(mTrackData.maxLapHR, mTrackData.HR);

    // Creating map
    SDK::TrackMapBuilder::GpsPoint newPoint{ mGps.latitude, mGps.longitude};
    // Add point to the track map
    mTrackMapBuilder.addPoint(newPoint);

    // Distance
    float distance = SDK::TrackMapBuilder::Distance(newPoint, mPrevGpsPoint) / 1000.0f;
    mPrevGpsPoint = newPoint;
    mTrackData.totalDistance += distance;
    mTrackData.lapDistance += distance;

    // Steps
    if (mStepCounter.initialSteps > 0) {
        int32_t total = static_cast<int32_t>(mStepCounter.steps - mStepCounter.initialSteps);
        int32_t diff = total - mTrackData.steps;
        mTrackData.steps = total;
        mTrackData.lapSteps += diff;
    }

    // Floors
    if (mFloorsCounter.initialFloors >= 0) {
        int32_t total = static_cast<int32_t>(mFloorsCounter.floors - mFloorsCounter.initialFloors);
        int32_t diff = total - mTrackData.floors;
        mTrackData.floors = total;
        mTrackData.lapFloors += diff;
    }

    // Elevation
    if (std::abs(mAltimeter.initialAltitude) > 0.01) {
        float total = mAltimeter.altitude - mAltimeter.initialAltitude;

        float diff = total - mTrackData.elevation;
        mTrackData.elevation = total;
        mTrackData.lapElevation += diff;

        if (diff > 0) {
            mAltimeter.ascent += diff;
            mAltimeter.lapAscent += diff;
        } else {
            mAltimeter.descent -= diff;
            mAltimeter.lapDescent -= diff;
        }
    }

    // Speed, km/h. Pace, seconds/km
    if (processTrackTimeDiff > 1 && distance > 0.001f) {
        mTrackData.speed = (distance / processTrackTimeDiff) * 3600.0f;
        mTrackData.pace = static_cast<int32_t>(3600.0f / mTrackData.speed);

        mTrackData.maxSpeed = std::max(mTrackData.maxSpeed, mTrackData.speed);
        mTrackData.maxLapSpeed = std::max(mTrackData.maxLapSpeed, mTrackData.speed);
    } else {
        mTrackData.speed = 0.0f;
        mTrackData.pace = 0;
    }
    if (mTrackData.totalTime > 1 && mTrackData.totalDistance > 0.01f) {
        mTrackData.avgSpeed = (mTrackData.totalDistance / mTrackData.totalTime) * 3600.0f;
        mTrackData.avgPace = static_cast<int32_t>(3600.0f / mTrackData.avgSpeed);
    } else {
        mTrackData.avgSpeed = 0.0f;
        mTrackData.avgPace = 0;
    }
    if (mTrackData.lapTime > 1 && mTrackData.lapDistance > 0.01f) {
        mTrackData.avgLapSpeed = (mTrackData.lapDistance / mTrackData.lapTime) * 3600.0f;
        mTrackData.lapPace = static_cast<int32_t>(3600.0f / mTrackData.avgLapSpeed);
    } else {
        mTrackData.avgLapSpeed = 0.0f;
        mTrackData.lapPace = 0;
    }
    

    // Save record to the FIT file
    ActivityWriter::RecordData fitRecord {};
    fitRecord.timestamp = utc;
    fitRecord.latitude = mGps.latitude;
    fitRecord.longitude = mGps.longitude;
    fitRecord.heartRate = static_cast<uint8_t>(mTrackData.HR);
    fitRecord.steps = static_cast<uint32_t>(mTrackData.steps);
    fitRecord.floors = static_cast<uint32_t>(mTrackData.floors);
    mActivityWriter.addRecord(fitRecord);

    mGSModel->sendToGUI(S2GEvent::TrackDataUpd{ mTrackData });

    // Next lap
    uint32_t lapNum = getCurrentLap();

    if (lapNum > mTrackData.lapNum) {

        // Save lap to the FIT file
        ActivityWriter::LapData fitLap {};
        fitLap.timeStart = utc - mTrackData.lapTime;
        fitLap.duration = mTrackData.lapTime;
        fitLap.elapsed = mTrackData.lapTime;
        fitLap.distance = mTrackData.lapDistance * 1000.0f; //m
        fitLap.speedAvg = mTrackData.avgLapSpeed / 3.6f;    // m/s
        fitLap.speedMax = mTrackData.maxLapSpeed / 3.6f;    // m/s
        fitLap.hrAvg = static_cast<uint8_t>(mTrackData.avgLapHR);
        fitLap.hrMax = static_cast<uint8_t>(mTrackData.maxLapHR);
        fitLap.ascent = mAltimeter.lapAscent;
        fitLap.descent = mAltimeter.lapDescent;
        fitLap.steps = static_cast<uint32_t>(mTrackData.lapSteps);
        fitLap.floors = static_cast<uint32_t>(mTrackData.lapFloors);
        mActivityWriter.addLap(fitLap);

        // Reset lap counters
        mTrackData.lapTime = 0;
        mTrackData.lapDistance = 0.0f;
        mTrackData.lapSteps = 0;
        mTrackData.lapFloors = 0;
        mTrackData.lapElevation = 0.0f;

        mAltimeter.lapAscent = 0.0f;
        mAltimeter.lapDescent = 0.0f;

        mTrackData.maxLapSpeed = 0.0f;
        mTrackData.avgLapSpeed = 0.0f;

        mTrackData.avgLapHR = 0.0f;
        mTrackData.maxLapHR = 0.0f;
        mHr.lapCnt = 0;
        mHr.lapSum = 0;



        mGSModel->sendToGUI(S2GEvent::LapEnded{ mTrackData.lapNum });
        mTrackData.lapNum = lapNum;
    }

}

void Service::stopTrack(bool discard)
{
    time_t timeNow = std::time(nullptr);
    if (!discard) {
        mSummary.utc = timeNow;
        mSummary.time = mTrackData.totalTime;
        mSummary.distance = mTrackData.totalDistance;
        mSummary.speedAvg = mTrackData.avgSpeed;
        mSummary.steps = mTrackData.steps;
        mSummary.elevation = static_cast<int32_t>(mTrackData.elevation);
        mSummary.paceAvg = mTrackData.avgPace;
        mSummary.hrMax = static_cast<int32_t>(mTrackData.maxHR);
        mSummary.hrAvg = static_cast<int32_t>(mTrackData.avgHR);
        mSummary.map = mTrackMapBuilder.build(70);

        // Save summary
        if (!mActivitySummarySerializer.save(mSummary)) {
            LOG_ERROR("Can't save activity summary\n");
        }
        mGSModel->sendToGUI(S2GEvent::Summary{ std::make_shared<const ActivitySummary>(mSummary) });

        // Create FIT file
        ActivityWriter::TrackData fitTrack{};
        fitTrack.timeStart = mTrackStartUTC;
        fitTrack.duration = mTrackData.totalTime;
        fitTrack.elapsed = mTrackData.totalTime;
        fitTrack.totalDistance = mTrackData.totalDistance * 1000.0f; //m
        fitTrack.speedAvg = mTrackData.avgSpeed / 3.6f;    // m/s
        fitTrack.speedMax = mTrackData.maxSpeed / 3.6f;    // m/s
        fitTrack.hrAvg = static_cast<uint8_t>(mTrackData.avgHR);
        fitTrack.hrMax = static_cast<uint8_t>(mTrackData.maxHR);
        fitTrack.ascent = mAltimeter.ascent;
        fitTrack.descent = mAltimeter.descent;
        fitTrack.steps = static_cast<uint32_t>(mTrackData.steps);
        fitTrack.floors = static_cast<uint32_t>(mTrackData.floors);
        mActivityWriter.stop(fitTrack);        
    } else {
        mActivityWriter.discard();
    }

    mTrackState = Track::State::INACTIVE;
    mGSModel->sendToGUI(S2GEvent::TrackStateUpd{ mTrackState });
}


Service::LapDivSource Service::getLapDivSource()
{
//    bool isImperial = mKernel.settings.isUnitsImperial();
//
//    float distance = isImperial ? Gui::Utils::km2mi(mSettings.alertDistance) : mSettings.alertDistance;
//    size_t distanceId = Gui::Utils::RoundToNearestIndex(Gui::kDistanceList,
//        Gui::Menu::Settings::Alerts::Distance::ID_COUNT, distance);
//
//    size_t stepId = Gui::Utils::RoundToNearestIndex(Gui::kStepsList,
//        Gui::Menu::Settings::Alerts::Steps::ID_COUNT, static_cast<float>(mSettings.alertSteps));
//
//    size_t timeId = Gui::Utils::RoundToNearestIndex(Gui::kTimeList,
//        Gui::Menu::Settings::Alerts::Time::ID_COUNT, static_cast<float>(mSettings.alertTime));
//
//
//    if (stepId != Gui::Menu::Settings::Alerts::Steps::ID_OFF) {
//        return LapDivSource::STEPS;
//    }
//
//    if (distanceId != Gui::Menu::Settings::Alerts::Distance::ID_OFF) {
//        return LapDivSource::DISTANCE;
//    }
//
//    if (timeId != Gui::Menu::Settings::Alerts::Time::ID_OFF) {
//        return LapDivSource::TIME;
//    }

    return LapDivSource::OFF;
}

uint32_t Service::getCurrentLap()
{
    // Determinate Lap split source
    LapDivSource divSource = getLapDivSource();

    uint32_t lapNum = 0;
    switch (divSource) {
    case LapDivSource::STEPS:
        lapNum = mTrackData.steps / mSettings.alertSteps;
        break;
    case LapDivSource::DISTANCE:
        lapNum = static_cast<uint32_t>(mTrackData.totalDistance / mSettings.alertDistance);
        break;
    case LapDivSource::TIME:
        lapNum = static_cast<uint32_t>(mTrackData.totalTime / (mSettings.alertTime * 60));
        break;

    case LapDivSource::OFF:
    default:
        return 0;
    }

    return lapNum;
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
