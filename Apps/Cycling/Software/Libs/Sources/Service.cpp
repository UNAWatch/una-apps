
#include "Service.hpp"

#include <ctime>
#include <cmath>
#include <memory>
#include <cstring>

#include "AppMenu.hpp"
#include "AppUtils.hpp"
#include "Settings.hpp"
#include "ActivitySummary.hpp"
#include "TrackInfo.hpp"
#include "icon_60x60.h"
#include "SDK/FirmwareVersion.hpp"
#include "SDK/Messages/SensorLayerMessages.hpp"
#include "SDK/Messages/MessageGuard.hpp"

#include "SDK/SensorLayer/DataParsers/SensorDataParserGpsLocation.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserGpsSpeed.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserGpsDistance.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserAltimeter.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRate.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserBatteryLevel.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserWristMotion.hpp"

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_INFO
#include "SDK/UnaLogger/Logger.h"

Service::Service(SDK::Kernel &kernel)
        : mKernel(kernel)
        , mGUIStarted(false)
        , mGuiSender(kernel)
        , mSettings{}
        , mSettingsSerializer(mKernel, "settings.json")
        , mSummary{}
        , mActivitySummarySerializer(mKernel, "Activity/summary.json")
        , mActivityWriter(mKernel, "Activity")
        , mTrackMapBuilder{}
        , mSensorGpsLocation(SDK::Sensor::Type::GPS_LOCATION, skInitialSamplePeriod, skSampleLatency)
        , mSensorGpsSpeed(SDK::Sensor::Type::GPS_SPEED, skInitialSamplePeriod, skSampleLatency)
        , mSensorGpsDistance(SDK::Sensor::Type::GPS_DISTANCE, skInitialSamplePeriod, skSampleLatency)
        , mSensorAltimeter(SDK::Sensor::Type::ALTIMETER, skInitialSamplePeriod, skSampleLatency)
        , mSensorHr(SDK::Sensor::Type::HEART_RATE, skInitialSamplePeriod, skSampleLatency)
        , mSensorBatteryLevel(SDK::Sensor::Type::BATTERY_LEVEL, skInitialSamplePeriod, skSampleLatency)
        , mSensorWristMotion(SDK::Sensor::Type::WRIST_MOTION, 300)
        , mName("Cycling")
{
}

Service::~Service()
{
    disconnect();   // Cleanup recourses
}

void Service::run()
{
    LOG_INFO("Started\n");

    // Get settings
    if (!mSettingsSerializer.load(mSettings)) {
        LOG_WARNING("Failed to load settings\n");
    }

    // Get summary
    if (!mActivitySummarySerializer.load(mSummary)) {
        LOG_WARNING("Failed to load activity summary\n");
    }

    uint32_t startTime = mKernel.sys.getTimeMs();
    bool firstFix = false;

    std::time_t processedUtc = 0;

    while (true) {
        SDK::MessageBase *msg;
        if (mKernel.comm.getMessage(msg, 500)) {
            // Command handling
            switch (msg->getType()) {

                // Kernel messages
                case SDK::MessageType::COMMAND_APP_STOP:
                    LOG_INFO("Force exit from the application\n");
                    disconnect();   // Cleanup recourses
                    if (mTrackState != Track::State::INACTIVE) {
                        stopTrack(std::time(nullptr), false);
                    }
                    // We must release message because this is the last event.
                    mKernel.comm.releaseMessage(msg);
                    return;

                case SDK::MessageType::COMMAND_APP_NOTIF_GUI_RUN:
                    LOG_INFO("GUI is now running\n");
                    onStartGUI();
                    break;

                case SDK::MessageType::COMMAND_APP_NOTIF_GUI_STOP:
                    LOG_INFO("GUI has stopped\n");
                    onStopGUI();
                    break;

                case SDK::MessageType::EVENT_GLANCE_START:
                    LOG_INFO("GLANCE is now running\n");
                    if (configGui()) {
                        createGuiControls();
                        mGlanceActive = true;
                    }
                    break;

                case SDK::MessageType::EVENT_GLANCE_STOP:
                    LOG_INFO("GLANCE has stopped\n");
                    mGlanceActive = false;
                    break;

                case SDK::MessageType::EVENT_GLANCE_TICK:
                    //LOG_DEBUG("GLANCE tick\n");
                    onGlanceTick();
                    break;


                // Custom  messages
                case CustomMessage::SETTINGS_SAVE:  {
                    LOG_DEBUG("SETTINGS_SAVE\n");
                    handleEvent(*static_cast<CustomMessage::SettingsUpd*>(msg));
                } break;

                case CustomMessage::TRACK_START:  {
                    LOG_DEBUG("TRACK_START\n");
                    handleEvent(*static_cast<CustomMessage::TrackStart*>(msg));
                } break;

                case CustomMessage::TRACK_STOP:  {
                    LOG_DEBUG("TRACK_STOP\n");
                    handleEvent(*static_cast<CustomMessage::TrackStop*>(msg));
                } break;


                // Sensors messages
                case SDK::MessageType::EVENT_SENSOR_LAYER_DATA: {
                    auto event = static_cast<SDK::Message::Sensor::EventData*>(msg);
                    SDK::Sensor::DataBatch batch(event->data, event->count, event->stride);
                    handleSensorsData(event->handle, batch);
                } break;

                default:
                    break;
            }
            // Release message after processing
            mKernel.comm.releaseMessage(msg);
        }


        // Periodic process
        if (mGUIStarted) {
            // Update time every second
            std::time_t utc = std::time(nullptr);

            if (processedUtc != utc) {

                if (mTrackState != Track::State::INACTIVE) {
                    if ((processedUtc > utc) || ((utc - processedUtc) > 5)) {
                        LOG_WARNING("Wrong UTC (%u -> %u). Restart track\n",
                                static_cast<uint32_t>(processedUtc),
                                static_cast<uint32_t>(utc));
                        stopTrack(processedUtc, false);
                        startTrack(utc);
                    }
                }

                processedUtc = utc;

                std::tm tmNow = toLocalTime(utc);

                mGuiSender.time(tmNow);
                mGuiSender.battery(static_cast<uint8_t>(mBattery.level));

                // Update GPS fix
                if (mPreviousGpsFixState != mGps.fix) {
                    mPreviousGpsFixState = mGps.fix;

                    if (!firstFix) {
                        notifyFirstFix();
                        firstFix = true;
                    }
                    mGuiSender.fix(mGps.fix);
                }

                if (mTrackState != Track::State::INACTIVE) {
                    processTrack(utc);
                }
            }
        } else if (mGlanceActive) {
            // do nothing
        } else {
            // Just wait some time to see if GUI starts
            if (mKernel.sys.getTimeMs() - startTime > 5000) {
                LOG_INFO("No activities, exiting service\n");
                return; // Exit app
            }
        }
    }
}

void Service::connectGps()
{
    if (!mSensorGpsLocation.isConnected()) {
        LOG_DEBUG("Connect to GPS sensor...\n");
        mSensorGpsLocation.connect();
    }
}

void Service::connectAll()
{
    if (!mIsSensorsConnected) {
        LOG_DEBUG("Connect to sensors...\n");

        mSensorBatteryLevel.connect();
        mSensorGpsSpeed.connect();
        mSensorGpsDistance.connect();
        mSensorAltimeter.connect();
        mSensorHr.connect();

        mIsSensorsConnected = true;
    }
}

void Service::disconnect()
{

    if (mIsSensorsConnected) {
        LOG_DEBUG("Disconnect from sensors...\n");

        mSensorHr.disconnect();
        mSensorAltimeter.disconnect();
        mSensorGpsSpeed.disconnect();
        mSensorGpsDistance.disconnect();
        mSensorBatteryLevel.disconnect();

        mIsSensorsConnected = false;
    }

    if (mSensorGpsLocation.isConnected()) {
        LOG_DEBUG("Disconnect from GPS sensor...\n");
        mSensorGpsLocation.disconnect();
    }
}


void Service::handleSensorsData(uint16_t handle, SDK::Sensor::DataBatch& data)
{
    if (mSensorGpsLocation.matchesDriver(handle)) {
        SDK::SensorDataParser::GpsLocation parser(data[0]);
        if (parser.isDataValid()) {
            mGps.timestamp = parser.getTimestamp();
            mGps.fix = parser.isCoordinatesValid();

            if (mGps.fix) { // Do not change position if no fix
                mGotFix = true;
                parser.getCoordinates(mGps.latitude, mGps.longitude, mGps.altitude);
            }
            LOG_DEBUG("Location: fix %u, lat %f, lon %f\n", mGps.fix, mGps.latitude, mGps.longitude);
        }
    } else if (mSensorGpsSpeed.matchesDriver(handle)) {
        SDK::SensorDataParser::GpsSpeed parser(data[0]);
        if (parser.isDataValid()) {
            mSpeed.speed = parser.getSpeed();
            mSpeed.timestamp = parser.getTimestamp();
            LOG_DEBUG("Speed:    %.2f m/s\n", mSpeed.speed);
        }
    } else if (mSensorGpsDistance.matchesDriver(handle)) {
        SDK::SensorDataParser::GpsDistance parser(data[0]);
        if (parser.isDataValid()) {
            mDistance.timestamp = parser.getTimestamp();
            float newValue = parser.getDistance();

            if (!mDistance.dataValid) {
                mDistance.initialDistance = newValue;
                mDistance.dataValid = true;
            }

            // The distance should increase monotonically
            if (newValue > mDistance.distance) {
                mDistance.distance = newValue;
            }
            LOG_DEBUG("Distance: %.2f m\n", mDistance.distance);
        }
    } else if (mSensorAltimeter.matchesDriver(handle)) {
        SDK::SensorDataParser::Altimeter parser(data[0]);
        if (parser.isDataValid()) {
            mAltimeter.timestamp = parser.getTimestamp();
            mAltimeter.altitude = parser.getAltitude();

            if (!mAltimeter.dataValid) {
                mAltimeter.initialAltitude = mAltimeter.altitude;
                mAltimeter.dataValid = true;
            }
            LOG_DEBUG("Altitude %.2f\n", mAltimeter.altitude);
        }
    } else if (mSensorHr.matchesDriver(handle)) {
        SDK::SensorDataParser::HeartRate parser(data[0]);
        if (parser.isDataValid()) {
            mHr.hr = parser.getBpm();
            mHr.trustLevel = parser.getTrustLevel();
            mHr.timestamp = parser.getTimestamp();
            if (mHr.hr > 1) {
                mHr.totalSum += mHr.hr;
                mHr.totalCnt++;
            }
            LOG_DEBUG("HR %.1f, TrustLevel %.1f\n", mHr.hr, mHr.trustLevel);
        }
    } else if (mSensorBatteryLevel.matchesDriver(handle)) {
        SDK::SensorDataParser::BatteryLevel parser(data[0]);
        if (parser.isDataValid()) {
            mBattery.level = parser.getCharge();
            LOG_DEBUG("Battery %.1f %%\n", mBattery.level);
        }
    } else if (mSensorWristMotion.matchesDriver(handle)) {
        SDK::SensorDataParser::WristMotion parser(data[0]);
        if (parser.isDataValid()) {
            LOG_DEBUG("Wrist Motion detected\n");
            auto bl = SDK::make_msg<SDK::Message::RequestBacklightSet>(mKernel);
            if (bl) {
                bl->brightness       = 100;
                bl->autoOffTimeoutMs = 5000;
                bl.send();
            }
        }
    }
}

void Service::onStartGUI()
{
    mGUIStarted = true;

    setCapabilities();

    // Subscribe to GPS to get fix
    connectGps();

    mSensorWristMotion.connect();

#if defined(SIMULATOR) || 0
    mGps.fix = true;
#endif

    sendInitialInfoToGui();
}

void Service::onStopGUI()
{
    mGUIStarted = false;

    mSensorWristMotion.disconnect();
}

void Service::handleEvent(const CustomMessage::TrackStart& event)
{
    startTrack(std::time(nullptr));
}

void Service::handleEvent(const CustomMessage::TrackStop& event)
{
    stopTrack(std::time(nullptr), event.discard);
}

void Service::handleEvent(const CustomMessage::SettingsUpd& event)
{
    bool updCaps = mSettings.phoneNotifEn != event.settings.phoneNotifEn;
    mSettings = event.settings;
    mSettingsSerializer.save(event.settings);

    if (updCaps) {
        setCapabilities();
    }
}

void Service::setCapabilities()
{
    auto *msg = mKernel.comm.allocateMessage<SDK::Message::RequestSetCapabilities>();
    if (msg) {
        msg->enPhoneNotification = mSettings.phoneNotifEn;
        msg->enUsbChargingScreen = false;
        msg->enMusicControl = true;
        mKernel.comm.sendMessage(msg);
        mKernel.comm.releaseMessage(msg);
    }
}

void Service::notifyFirstFix()
{
    auto *backlightMsg = mKernel.comm.allocateMessage<SDK::Message::RequestBacklightSet>();
    if (backlightMsg) {
        backlightMsg->autoOffTimeoutMs = skBacklightTimeout;
        backlightMsg->brightness = 100;
        mKernel.comm.sendMessage(backlightMsg);
        mKernel.comm.releaseMessage(backlightMsg);
    }

    auto *buzzerMsg = mKernel.comm.allocateMessage<SDK::Message::RequestBuzzerPlay>();
    if (buzzerMsg) {
        buzzerMsg->notes[0].volume = 100;
        buzzerMsg->notes[0].time  = 100;
        buzzerMsg->notesCount = 1;

        mKernel.comm.sendMessage(buzzerMsg);
        mKernel.comm.releaseMessage(buzzerMsg);
    }

    auto *vibroMsg = mKernel.comm.allocateMessage<SDK::Message::RequestVibroPlay>();
    if (vibroMsg) {

        vibroMsg->notes[0].effect = SDK::Message::RequestVibroPlay::Effect::STRONG_CLICK_100;
        vibroMsg->notesCount = 1;

        mKernel.comm.sendMessage(vibroMsg);
        mKernel.comm.releaseMessage(vibroMsg);
    }
}

void Service::notifyLapEnd()
{
    auto *backlightMsg = mKernel.comm.allocateMessage<SDK::Message::RequestBacklightSet>();
    if (backlightMsg) {
        backlightMsg->autoOffTimeoutMs = skBacklightTimeout;
        backlightMsg->brightness = 100;
        mKernel.comm.sendMessage(backlightMsg);
        mKernel.comm.releaseMessage(backlightMsg);
    }

    auto *buzzerMsg = mKernel.comm.allocateMessage<SDK::Message::RequestBuzzerPlay>();
    if (buzzerMsg) {
        buzzerMsg->notes[0].volume = 100;
        buzzerMsg->notes[0].time  = 100;
        buzzerMsg->notesCount = 1;

        mKernel.comm.sendMessage(buzzerMsg);
        mKernel.comm.releaseMessage(buzzerMsg);
    }

    auto *vibroMsg = mKernel.comm.allocateMessage<SDK::Message::RequestVibroPlay>();
    if (vibroMsg) {

        vibroMsg->notes[0].effect = SDK::Message::RequestVibroPlay::Effect::ALERT_750MS_100;
        vibroMsg->notesCount = 1;

        mKernel.comm.sendMessage(vibroMsg);
        mKernel.comm.releaseMessage(vibroMsg);
    }
}

void Service::notifyNewActivity()
{
    auto *msg = mKernel.comm.allocateMessage<SDK::Message::CommandAppNewActivity>();
    if (msg) {
        mKernel.comm.sendMessage(msg);
        mKernel.comm.releaseMessage(msg);
    }
}

std::tm Service::toLocalTime(std::time_t utc)
{
    std::tm tmNow{};
#if WIN32
    localtime_s(&tmNow, &utc);
#else
    localtime_r(&utc, &tmNow);
#endif
    return tmNow;
}


void Service::sendInitialInfoToGui()
{
    // Settings
    std::array<uint8_t, 4> hrThresholds = kHrThresholdsDefault;

    auto *msg = mKernel.comm.allocateMessage<SDK::Message::RequestSystemSettings>();
    if (msg) {
        if(mKernel.comm.sendMessage(msg, 100) && msg->getResult() == SDK::MessageResult::SUCCESS) {
            mUnits = msg->imperialUnits;
            if (msg->heartRateCount >= 4) {
                for (uint8_t i = 0; i < 4; i++) {
                    hrThresholds[i] = msg->heartRateTh[i];
                }
            }
        }
        mKernel.comm.releaseMessage(msg);
    }

    mGuiSender.settingsUpd(mSettings, mUnits, hrThresholds);

    // Summary
    mGuiSender.summary(std::make_shared<const ActivitySummary>(mSummary));

    // Battery level
    mGuiSender.battery(static_cast<uint8_t>(mBattery.level));
}

void Service::startTrack(std::time_t utc)
{
    // Reset data
    memset(&mTrackData, 0, sizeof(mTrackData));
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

    connectAll();

    ActivityWriter::AppInfo info{};
    info.timestamp = utc;
    info.appVersion = SDK::ParseVersion(BUILD_VERSION).u32;
    info.devID = DEV_ID;
    info.appID = APP_ID;
    mActivityWriter.start(info);

    mTrackState = Track::State::ACTIVE;

    mGuiSender.trackState(mTrackState);
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
    if (mGotFix) {
        mTrackMapBuilder.addPoint(newPoint);
    }

    // Distance, m
    if (mDistance.dataValid) {
        float total = mDistance.distance - mDistance.initialDistance;
        float diff = total - mTrackData.distance;
        mTrackData.distance = total;
        mTrackData.lapDistance += diff;
    }

    // Elevation, m
    if (mAltimeter.dataValid) {
        float diff = mAltimeter.altitude - mTrackData.elevation;
        if (std::abs(mTrackData.elevation) < 0.01) {
            diff = 0;
        }
        mTrackData.elevation = mAltimeter.altitude;
//        mTrackData.lapElevation = ; // not used

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
    std::time_t timeDiff = std::abs(utc - mTrackProcessTimestamp);
    if (timeDiff < (skSamplePeriod / 1000)) { // in seconds
        if (trackTimeDiff > 0) {
            mGuiSender.trackData(mTrackData);
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
    fitRecord.gotFix    = mGotFix;
    fitRecord.latitude  = mGps.latitude;
    fitRecord.longitude = mGps.longitude;
    fitRecord.heartRate = mTrackData.hrTrustLevel >= 1.0 ? mTrackData.HR : 0.0f;
    fitRecord.altitude  = mAltimeter.altitude;
	fitRecord.speed     = mTrackData.speed;
    mActivityWriter.addRecord(fitRecord);

    mGuiSender.trackData(mTrackData);

    mSessionNotEmpty = true;    // Session has at least one record
    mLapNotEmpty = true;        // Lap has at least one record

    // Next lap
    uint32_t lapNum = getCurrentLap();

    if (lapNum > mTrackData.lapNum) {

        saveLap(utc);

        mGuiSender.lapEnd(mTrackData.lapNum);

        mTrackData.lapNum = lapNum;

        notifyLapEnd();
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
    mActivityWriter.addLap(fitLap);

    // Reset lap counters
    mTrackData.lapTime = 0;
    mTrackData.lapDistance = 0.0f;
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
        mSummary.elevation = mTrackData.elevation;
        mSummary.paceAvg = mTrackData.avgPace;
        mSummary.hrMax = mTrackData.maxHR;
        mSummary.hrAvg = mTrackData.avgHR;
        mSummary.map = mTrackMapBuilder.build(70);

        // Save summary
        if (!mActivitySummarySerializer.save(mSummary)) {
            LOG_ERROR("Can't save activity summary\n");
        }
        mGuiSender.summary(std::make_shared<const ActivitySummary>(mSummary));

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
        mActivityWriter.stop(fitTrack);

        notifyNewActivity();
    } else {
        mActivityWriter.discard();
    }

    mTrackState = Track::State::INACTIVE;
    mGuiSender.trackState(mTrackState);

    disconnect();
}


Service::LapDivSource Service::getLapDivSource()
{

    float distance = mUnits ? App::Utils::km2mi(mSettings.alertDistance) : mSettings.alertDistance;
    size_t distanceId = App::Menu::RoundToNearestIndex(App::Menu::kDistanceList,
        App::Menu::Settings::Alerts::Distance::ID_COUNT, distance);

    size_t timeId = App::Menu::RoundToNearestIndex(App::Menu::kTimeList,
        App::Menu::Settings::Alerts::Time::ID_COUNT, static_cast<float>(mSettings.alertTime));

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

void Service::onGlanceTick()
{
    if (mGlanceUI.isInvalid()) {
        auto* upd = mKernel.comm.allocateMessage<SDK::Message::RequestGlanceUpdate>();
        if (upd) {
            upd->name = mName;
            upd->controls = mGlanceUI.data();
            upd->controlsNumber = static_cast<uint32_t>(mGlanceUI.size());

            mKernel.comm.sendMessage(upd, 100);
            mKernel.comm.releaseMessage(upd);
        }

        mGlanceUI.setValid();
   }
}

bool Service::configGui()
{
    bool status = false;
    // Get Glance configuration
    auto* gc = mKernel.comm.allocateMessage<SDK::Message::RequestGlanceConfig>();
    if (gc) {
        if (mKernel.comm.sendMessage(gc, 100) && gc->getResult() == SDK::MessageResult::SUCCESS) {
            if (gc->maxControls >= 3) {
                mMaxControls = gc->maxControls;
                mGlanceUI.setWidth(gc->width);
                mGlanceUI.setHeight(gc->height);
                status = true;
            }
        }
        mKernel.comm.releaseMessage(gc);
    }

    return status;
}

void Service::createGuiControls()
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

    if (mSummary.utc != 0) {
        std::tm tmNow{};
#if WIN32
        localtime_s(&tmNow, &mSummary.utc);
#else
        localtime_r(&mSummary.utc, &tmNow);
#endif
        mGlanceTime.print("%s %d, %d:%02d", App::Utils::DayShort[tmNow.tm_wday], tmNow.tm_mday, tmNow.tm_hour, tmNow.tm_min);
    }
}
