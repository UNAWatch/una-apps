#include "Service.hpp"
#include "GSModelEvents/G2SEvents.hpp"

#include <ctime>
#include <cmath>
#include <memory>
#include <cstring>

#include "AppMenu.hpp"
#include "AppUtils.hpp"

#include "Settings.hpp"
#include "ActivitySummary.hpp"
#include "TrackInfo.hpp"

#include "SDK/FirmwareVersion.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserGpsLocation.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserGpsSpeed.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserGpsDistance.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserAltimeter.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRate.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserStepCounter.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserFloorCounter.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserBatteryLevel.hpp"

#include "icon_60x60.h"

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_INFO
#include "SDK/UnaLogger/Logger.h"

Service::Service()
        : mKernel(SDK::KernelProviderService::GetInstance().getKernel())
        , mGSModel(*this)
        , mTerminate(false)
        , mGUIStarted(false)
        , mSettings{}
        , mSettingsSerializer(mKernel, "settings.json")
        , mSummary{}
        , mActivitySummarySerializer(mKernel, "Activity/summary.json")
        , mActivityWriter(mKernel, "Activity")
        , mGpsLocationSensor(SDK::Sensor::Type::GPS_LOCATION, this, skInitialSamplePeriod, skSampleLatency)
        , mGpsSpeedSensor(SDK::Sensor::Type::GPS_SPEED, this, skInitialSamplePeriod, skSampleLatency)
        , mGpsDistanceSensor(SDK::Sensor::Type::GPS_DISTANCE, this, skInitialSamplePeriod, skSampleLatency)
        , mStepCounterSensor(SDK::Sensor::Type::STEP_COUNTER, this, skInitialSamplePeriod, skSampleLatency)
        , mFloorCounterSensor(SDK::Sensor::Type::FLOOR_COUNTER, this, skInitialSamplePeriod, skSampleLatency)
        , mAltimeterSensor(SDK::Sensor::Type::ALTIMETER, this, skInitialSamplePeriod, skSampleLatency)
        , mHrSensor(SDK::Sensor::Type::HEART_RATE, this, skInitialSamplePeriod, skSampleLatency)
        , mBatteryLevelSensor(SDK::Sensor::Type::BATTERY_LEVEL, this, skInitialSamplePeriod, skSampleLatency)
{
    int16_t w;
    int16_t h;
    mKernel.app.getGlanceArea(w, h);
    mGlanceUI.setWidth(w);
    mGlanceUI.setHeight(h);
    mKernel.app.registerGlance(this);

    createGlanceGUI();
}

void Service::run()
{
    LOG_INFO("Started\n");

    // Get settings
    if (!mSettingsSerializer.load(mSettings)) {
        LOG_ERROR("Failed to load settings\n");
    }

    // Get summary
    if (!mActivitySummarySerializer.load(mSummary)) {
        LOG_ERROR("Failed to load activity summary\n");
    } else {
        time_t utc = mSummary.utc; //  Last activity UTC time
        std::tm tmNow{};
#if WIN32
        localtime_s(&tmNow, &utc);
#else
        localtime_r(&utc, &tmNow);
#endif
        mGlanceTime.print("%s %d, %d:%02d", App::Utils::DayShort[tmNow.tm_wday], tmNow.tm_mday, tmNow.tm_hour, tmNow.tm_min);
    }

    uint32_t startTime = mKernel.system.getTimeMs();
    bool firstFix = false;

    while (!mTerminate) {
        mGSModel.process(1000);

        if (mGUIStarted) {

            // Update time every second
            std::time_t utc = std::time(nullptr);
            std::tm tmNow{};
            #if WIN32
                localtime_s(&tmNow, &utc);
            #else
                localtime_r(&utc, &tmNow);
            #endif
            mGSModel.post(S2GEvent::Time{ tmNow });

            mGSModel.post(S2GEvent::Battery{ static_cast<uint8_t>(mBattery.level) });

            // Update GPS fix
            if (mPreviousGpsFixState != mGps.fix) {
                mPreviousGpsFixState = mGps.fix;

                if (!firstFix) {
                    mKernel.backlight.on(3000);
                    mKernel.buzzer.play();
                    mKernel.vibro.play();
                    firstFix = true;
                }
                mGSModel.post(S2GEvent::GpsFix{ mGps.fix });
            }

            if (mTrackState != Track::State::INACTIVE) {
                processTrack(utc);
            }


        } else if (mGlanceActive) {
            // do nothing
        } else {
            // Just wait some time to see if GUI starts
            if (mKernel.system.getTimeMs() - startTime > 5000) {
                break;
            }
        }
    }

    if (mTrackState != Track::State::INACTIVE) {
        stopTrack(std::time(nullptr), false);
    }

    // Unsubscribe from sensors
    mGpsLocationSensor.disconnect();
    mGpsSpeedSensor.disconnect();
    mGpsDistanceSensor.disconnect();
    mHrSensor.disconnect();
    mStepCounterSensor.disconnect();
    mAltimeterSensor.disconnect();
    mFloorCounterSensor.disconnect();
    mBatteryLevelSensor.disconnect();

    LOG_INFO("Stopped\n");
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

    // Subscribe to GPS to get fix
    mGpsLocationSensor.connect();

#if defined(SIMULATOR) || 0
    mGps.fix = true;
#else
    if (mSettings.debugSkipGpsFix) {
        mGps.fix = true;
    }
#endif

    sendInitialInfoToGui();
    
    mGSModel.abortProcessWait();
}

void Service::onStopGUI()
{
    LOG_INFO("GUI stopped\n");
    mGUIStarted = false;
    mGSModel.abortProcessWait();
}

void Service::onSdlNewData(const SDK::Interface::ISensorDriver*              sensor,
                           const std::vector< SDK::Interface::ISensorData*>& data,
                           bool                                              first)
{
    // We are only interested in the last sample
    const size_t sampleNum = data.size();
    if (sampleNum == 0) {
        return;
    }
    
    const SDK::Interface::ISensorData& sample = *data[sampleNum - 1];

    if (mGpsLocationSensor.matchesDriver(sensor)) {
        SDK::SensorDataParser::GpsLocation parser{ sample };
        if (parser.isDataValid()) {
            mGps.timestamp = parser.getTimestamp();
            mGps.fix = parser.isCoordinatesValid();

            if (mGps.fix) { // Do not change position if no fix
                parser.getCoordinates(mGps.latitude, mGps.longitude, mGps.altitude);
            }
            LOG_DEBUG("Location: fix %u, lat %f, lon %f\n", mGps.fix, mGps.latitude, mGps.longitude);
        }
    } else if (mGpsSpeedSensor.matchesDriver(sensor)) {
        SDK::SensorDataParser::GpsSpeed parser{ sample };
        if (parser.isDataValid()) {
            mSpeed.speed = parser.getSpeed();
            mSpeed.timestamp = parser.getTimestamp();
            LOG_DEBUG("Speed:    %.2f m/s\n", mSpeed.speed);
        }
    } else if (mGpsDistanceSensor.matchesDriver(sensor)) {
        SDK::SensorDataParser::GpsDistance parser{ sample };
        if (parser.isDataValid()) {
            mDistance.distance = parser.getDistance();
            mDistance.timestamp = parser.getTimestamp();

            if (!mDistance.dataValid) {
                mDistance.initialDistance = mDistance.distance;
                mDistance.dataValid = true;
            }
            LOG_DEBUG("Distance: %.2f m\n", mDistance.distance);
        }
    } else if (mStepCounterSensor.matchesDriver(sensor)) {
        SDK::SensorDataParser::StepCounter sc{ sample };
        if (sc.isDataValid()) {
            mStepCounter.timestamp = sc.getTimestamp();
            mStepCounter.steps = sc.getStepCount();

            if (!mStepCounter.dataValid) {
                mStepCounter.initialSteps = mStepCounter.steps;
                mStepCounter.dataValid = true;
            }
            LOG_DEBUG("Steps %u\n", mStepCounter.steps);
        }
    } else if (mFloorCounterSensor.matchesDriver(sensor)) {
        SDK::SensorDataParser::FloorCounter fc{ sample };
        if (fc.isDataValid()) {
            mFloorsCounter.timestamp = fc.getTimestamp();
            mFloorsCounter.floors = fc.getFloorsUp() + fc.getFloorsDown();

            if (!mFloorsCounter.dataValid) {
                mFloorsCounter.initialFloors = mFloorsCounter.floors;
                mFloorsCounter.dataValid = true;
            }
            LOG_DEBUG("Floors %u\n", mFloorsCounter.floors);
        }
    } else if (mAltimeterSensor.matchesDriver(sensor)) {
        SDK::SensorDataParser::Altimeter alt{ sample };
        if (alt.isDataValid()) {
            mAltimeter.timestamp = alt.getTimestamp();
            mAltimeter.altitude = alt.getAltitude();

            if (!mAltimeter.dataValid) {
                mAltimeter.initialAltitude = mAltimeter.altitude;
                mAltimeter.dataValid = true;
            }
            LOG_DEBUG("Altitude %.2f\n", mAltimeter.altitude);
        }
    } else if (mHrSensor.matchesDriver(sensor)) {
        SDK::SensorDataParser::HeartRate hr{ sample };
        if (hr.isDataValid()) {
            mHr.hr = hr.getBpm();
            mHr.trustLevel = hr.getTrustLevel();
            mHr.timestamp = sample.getTimestamp();

            mHr.totalSum += mHr.hr;
            mHr.totalCnt++;
            LOG_DEBUG("HR %.1f, TrustLevel %.1f\n", mHr.hr, mHr.trustLevel);
        }
    } else if (mBatteryLevelSensor.matchesDriver(sensor)) {
        SDK::SensorDataParser::BatteryLevel lvl{ sample };
        if (lvl.isDataValid()) {
            mBattery.level = lvl.getCharge();
            LOG_DEBUG("Battery %.1f %%\n", mBattery.level);
        }
    }
}

void Service::handleEvent(const G2SEvent::TrackStart& event)
{
    LOG_DEBUG("G2SEvent::TrackStart\n");
    startTrack(std::time(nullptr));
    mGSModel.abortProcessWait();
}

void Service::handleEvent(const G2SEvent::TrackStop& event)
{
    LOG_DEBUG("G2SEvent::TrackStop\n");
    stopTrack(std::time(nullptr), event.discard);
    mGSModel.abortProcessWait();
}

void Service::handleEvent(const G2SEvent::SettingsSave& event)
{
    LOG_DEBUG("G2SEvent::SettingsSave\n");
    mSettings = event.settings;
    mSettingsSerializer.save(event.settings);
}

void Service::sendInitialInfoToGui()
{
    // Settings
    mGSModel.post(S2GEvent::SettingsUpd{ mSettings, mKernel.settings.isUnitsImperial(), mKernel.settings.getHrThresholds() });

    // Summary
    mGSModel.post(S2GEvent::Summary{ std::make_shared<const ActivitySummary>(mSummary) });

    // Battery level
    mGSModel.post(S2GEvent::Battery{ static_cast<uint8_t>(mBattery.level) });
}

void Service::startTrack(std::time_t utc)
{
    // Reset data
    memset(&mTrackData, 0, sizeof(mTrackData));
    mStepCounter.reset();
    mFloorsCounter.reset();
    mAltimeter.reset();
    mHr.reset();
    mSpeed.reset();
    mDistance.reset();

    mSessionNotEmpty = false;
    mLapNotEmpty = false;

    // Configure TrackMapBuilder
    mTrackMapBuilder.reset();

    SDK::TrackMapBuilder::GpsPoint startGpsPoint{ mGps.latitude, mGps.longitude };
    mTrackMapBuilder.setDistanceThreshold(startGpsPoint, skMapDistanceThreshold);

    // Determinate Lap split source
    mLapDivSource = getLapDivSource();

    mTrackStartUTC = utc;
    mTrackProcessTimestamp = mTrackStartUTC;

    mBatteryLevelSensor.connect();
    mGpsSpeedSensor.connect();
    mGpsDistanceSensor.connect();
    mStepCounterSensor.connect();
    mFloorCounterSensor.connect();
    mAltimeterSensor.connect();
    mHrSensor.connect();

    ActivityWriter::AppInfo info{};
    info.timestamp = utc;
    info.appVersion = SDK::ParseVersion(BUILD_VERSION).u32;
    info.devID = DEV_ID;
    info.appID = APP_ID;
    mActivityWriter.start(info);

    mTrackState = Track::State::ACTIVE;

    mGSModel.post(S2GEvent::TrackStateUpd{ mTrackState });
}

void Service::processTrack(std::time_t utc)
{
    // Process time
    std::time_t trackTime = static_cast<std::time_t>(std::difftime(utc, mTrackStartUTC));
    std::time_t trackTimeDiff = static_cast<std::time_t>(std::difftime(trackTime, mTrackData.totalTime));
    mTrackData.totalTime = trackTime;
    mTrackData.lapTime += trackTimeDiff;

    // Process sensors

    // HR
    mTrackData.HR = mHr.hr;
    mTrackData.hrTrustLevel = mHr.trustLevel;
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

    // GPS
    // Creating map
    SDK::TrackMapBuilder::GpsPoint newPoint{ mGps.latitude, mGps.longitude };
    // Add point to the track map
    mTrackMapBuilder.addPoint(newPoint);

    // Distance, m
    if (mDistance.dataValid) {
        float total = mDistance.distance - mDistance.initialDistance;
        float diff = total - mTrackData.distance;
        mTrackData.distance = total;
        mTrackData.lapDistance += diff;
    }

    // Steps
    if (mStepCounter.dataValid) {
        uint32_t total = mStepCounter.steps - mStepCounter.initialSteps;
        uint32_t diff = total - mTrackData.steps;
        mTrackData.steps = total;
        mTrackData.lapSteps += diff;
    }

    // Floors
    if (mFloorsCounter.dataValid) {
        uint32_t total = mFloorsCounter.floors - mFloorsCounter.initialFloors;
        uint32_t diff = total - mTrackData.floors;
        mTrackData.floors = total;
        mTrackData.lapFloors += diff;
    }

    // Elevation, m
    if (mAltimeter.dataValid) {
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

    // Speed, m/s. Pace, s/m
    mTrackData.speed = mSpeed.speed;
    if (mTrackData.speed > 0.01f) {
        mTrackData.pace = 1.0f / mTrackData.speed;
        mTrackData.maxSpeed = std::max(mTrackData.maxSpeed, mTrackData.speed);
        mTrackData.maxLapSpeed = std::max(mTrackData.maxLapSpeed, mTrackData.speed);
    } else {
        mTrackData.pace = 0.0f;
    }

    if (mTrackData.totalTime > 1 && mTrackData.distance > 0.01f) {
        mTrackData.avgSpeed = (mTrackData.distance / mTrackData.totalTime);
        mTrackData.avgPace = 1.0f / mTrackData.avgSpeed;
    } else {
        mTrackData.avgSpeed = 0.0f;
        mTrackData.avgPace = 0;
    }
    if (mTrackData.lapTime > 1 && mTrackData.lapDistance > 0.01f) {
        mTrackData.avgLapSpeed = (mTrackData.lapDistance / mTrackData.lapTime);
        mTrackData.lapPace = 1.0f / mTrackData.avgLapSpeed;
    } else {
        mTrackData.avgLapSpeed = 0.0f;
        mTrackData.lapPace = 0.0f;
    }

#if 1
    std::time_t timeDiff = static_cast<std::time_t>(std::difftime(utc, mTrackProcessTimestamp));
    if (timeDiff < (skSamplePeriod / 1000)) { // in seconds
        if (trackTimeDiff > 0) {
            mGSModel.post(S2GEvent::TrackDataUpd{ mTrackData });
        }
        return;
    }
#else
    // Debug. Save data every second
#endif
    mTrackProcessTimestamp = utc;

    // Save record to the FIT file
    ActivityWriter::RecordData fitRecord{};
    fitRecord.timestamp = utc;
    fitRecord.latitude  = mGps.latitude;
    fitRecord.longitude = mGps.longitude;
    fitRecord.heartRate = mTrackData.hrTrustLevel >= 1.0 ? mTrackData.HR : 0.0f;
    fitRecord.altitude  = mAltimeter.altitude;
	fitRecord.speed     = mTrackData.speed;
    mActivityWriter.addRecord(fitRecord);

    mGSModel.post(S2GEvent::TrackDataUpd{ mTrackData });

    mSessionNotEmpty = true;    // Session has at least one record
    mLapNotEmpty = true;        // Lap has at least one record

    // Next lap
    uint32_t lapNum = getCurrentLap();

    if (lapNum > mTrackData.lapNum) {

        saveLap(utc);

        mGSModel.post(S2GEvent::LapEnded{ mTrackData.lapNum });
        mTrackData.lapNum = lapNum;

        // Backlight
        mKernel.backlight.on(3000);

        // Vibro & buzzer
        mKernel.vibro.play(SDK::Interface::IVibro::ALERT_750MS_100);
        mKernel.buzzer.play();
    }

}

void Service::saveLap(std::time_t utc)
{
    // Save lap to the FIT file
    ActivityWriter::LapData fitLap{};
    fitLap.timestamp = utc;
    fitLap.timeStart = utc - mTrackData.lapTime;
    fitLap.duration = mTrackData.lapTime;
    fitLap.elapsed = mTrackData.lapTime;
    fitLap.distance = mTrackData.lapDistance;   // m
    fitLap.speedAvg = mTrackData.avgLapSpeed;   // m/s
    fitLap.speedMax = mTrackData.maxLapSpeed;   // m/s
    fitLap.hrAvg = mTrackData.avgLapHR;
    fitLap.hrMax = mTrackData.maxLapHR;
    fitLap.ascent = mAltimeter.lapAscent;
    fitLap.descent = mAltimeter.lapDescent;
    fitLap.steps = mTrackData.lapSteps;
    fitLap.floors = mTrackData.lapFloors;
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

    mLapNotEmpty = false;
}

void Service::stopTrack(std::time_t utc, bool discard)
{
    if (!discard && mSessionNotEmpty) {

        if (mLapNotEmpty) {
            saveLap(utc);
            mTrackData.lapNum++;
        }

        mSummary.utc = utc;
        mSummary.time = mTrackData.totalTime;
        mSummary.distance = mTrackData.distance;
        mSummary.speedAvg = mTrackData.avgSpeed;
        mSummary.steps = mTrackData.steps;
        mSummary.elevation = mTrackData.elevation;
        mSummary.paceAvg = mTrackData.avgPace;
        mSummary.hrMax = mTrackData.maxHR;
        mSummary.hrAvg = mTrackData.avgHR;
        mSummary.map = mTrackMapBuilder.build(70);

        // Save summary
        if (!mActivitySummarySerializer.save(mSummary)) {
            LOG_ERROR("Can't save activity summary\n");
        }
        mGSModel.post(S2GEvent::Summary{ std::make_shared<const ActivitySummary>(mSummary) });

        // Save FIT file
        ActivityWriter::TrackData fitTrack{};
        fitTrack.timestamp = utc;
        fitTrack.timeStart = mTrackStartUTC;
        fitTrack.duration = mTrackData.totalTime;
        fitTrack.elapsed = mTrackData.totalTime;
        fitTrack.distance = mTrackData.distance;    // m
        fitTrack.speedAvg = mTrackData.avgSpeed;    // m/s
        fitTrack.speedMax = mTrackData.maxSpeed;    // m/s
        fitTrack.hrAvg = mTrackData.avgHR;
        fitTrack.hrMax = mTrackData.maxHR;
        fitTrack.ascent = mAltimeter.ascent;
        fitTrack.descent = mAltimeter.descent;
        fitTrack.steps = mTrackData.steps;
        fitTrack.floors = mTrackData.floors;
        mActivityWriter.stop(fitTrack);
    } else {
        mActivityWriter.discard();
    }

    mTrackState = Track::State::INACTIVE;
    mGSModel.post(S2GEvent::TrackStateUpd{ mTrackState });

    mGpsLocationSensor.disconnect();
    mGpsSpeedSensor.disconnect();
    mGpsDistanceSensor.disconnect();
    mHrSensor.disconnect();
    mStepCounterSensor.disconnect();
    mAltimeterSensor.disconnect();
    mFloorCounterSensor.disconnect();
    mBatteryLevelSensor.disconnect();
}


Service::LapDivSource Service::getLapDivSource()
{
    bool isImperial = mKernel.settings.isUnitsImperial();

    size_t stepId = App::Menu::RoundToNearestIndex(App::Menu::kStepsList,
        App::Menu::Settings::Alerts::Steps::ID_COUNT, static_cast<float>(mSettings.alertSteps));

    float distance = isImperial ? App::Utils::km2mi(mSettings.alertDistance) : mSettings.alertDistance;
    size_t distanceId = App::Menu::RoundToNearestIndex(App::Menu::kDistanceList,
        App::Menu::Settings::Alerts::Distance::ID_COUNT, distance);

    size_t timeId = App::Menu::RoundToNearestIndex(App::Menu::kTimeList,
        App::Menu::Settings::Alerts::Time::ID_COUNT, static_cast<float>(mSettings.alertTime));


    if (stepId != App::Menu::Settings::Alerts::Steps::ID_OFF) {
        return LapDivSource::STEPS;
    }

    if (distanceId != App::Menu::Settings::Alerts::Distance::ID_OFF) {
        return LapDivSource::DISTANCE;
    }

    if (timeId != App::Menu::Settings::Alerts::Time::ID_OFF) {
        return LapDivSource::TIME;
    }

    return LapDivSource::OFF;
}

uint32_t Service::getCurrentLap()
{
    uint32_t lapNum = 0;
    switch (mLapDivSource) {
    case LapDivSource::STEPS:
        lapNum = mTrackData.steps / mSettings.alertSteps;
        break;
    case LapDivSource::DISTANCE:
        lapNum = static_cast<uint32_t>(mTrackData.distance / (mSettings.alertDistance * 1000.0f));
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

SDK::Interface::IGlance::Info Service::glanceGetInfo()
{
    mGlanceActive = true;

    IGlance::Info info{};
    info.altname = "Hiking";
    info.count = static_cast<uint8_t>(mGlanceUI.size());
    info.ctrls = mGlanceUI.data();
    return info;
}

void Service::glanceUpdate()
{
    // Nothing changed
}

void Service::glanceClose()
{
    mGlanceActive = false;
}

void Service::createGlanceGUI()
{
    mGlanceUI.createImage().init({20, 0}, {60, 60}, ICON_60X60_ABGR2222);

    mGlanceTitle = mGlanceUI.createText();
    mGlanceTitle.pos({ 81, 0 }, { 125, 25 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_20)
        .color(GlanceColor_t::GLANCE_COLOR_TEAL)
        .setText("Last Activity")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_LEFT);

    mGlanceTime = mGlanceUI.createText();
    mGlanceTime.pos({ 34, 34 }, { 172, 23 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_ITALIC_18)
        .color(GlanceColor_t::GLANCE_COLOR_WHITE)
        .setText("No activities")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_RIGHT);
}
