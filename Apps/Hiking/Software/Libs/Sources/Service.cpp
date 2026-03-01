
#include "Service.hpp"

#include <ctime>
#include <cmath>
#include <memory>
#include <cstring>
#include <array>

#include "AppMenu.hpp"
#include "AppUtils.hpp"
#include "Settings.hpp"
#include "ActivitySummary.hpp"
#include "TrackInfo.hpp"
#include "icon_60x60.h"
#include "SDK/Tools/FirmwareVersion.hpp"
#include "SDK/Messages/SensorLayerMessages.hpp"
#include "SDK/Messages/MessageGuard.hpp"

#include "SDK/SensorLayer/DataParsers/SensorDataParserGpsLocation.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserGpsSpeed.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserGpsDistance.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserPressure.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRate.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserStepCounter.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserFloorCounter.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserBatteryLevel.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserBatteryMetrics.hpp"
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
        , mSensorStepCounter(SDK::Sensor::Type::STEP_COUNTER, skInitialSamplePeriod, skSampleLatency)
        , mSensorFloorCounter(SDK::Sensor::Type::FLOOR_COUNTER, skInitialSamplePeriod, skSampleLatency)
        , mSensorPressure(SDK::Sensor::Type::PRESSURE, skInitialSamplePeriod, skSampleLatency)
        , mSensorHr(SDK::Sensor::Type::HEART_RATE, skInitialSamplePeriod, skSampleLatency)
        , mSensorBatteryLevel(SDK::Sensor::Type::BATTERY_LEVEL, skInitialSamplePeriod, skSampleLatency)
        , mSensorBatteryMetrics(SDK::Sensor::Type::BATTERY_METRICS, skInitialSamplePeriod, skSampleLatency)
        , mSensorWristMotion(SDK::Sensor::Type::WRIST_MOTION, 300)
        , mTimeTracker(kernel)
        , mAltitudeFilter(0.8f)
        , mName("Hiking")
{
    mTimeCounter.init();
    mDistanceCounter.init();
    mSpeedCounter.init(0.5f, 300.0f);
    mHrCounter.init(20.0f, 300.0f);
    mAltitudeCounter.init(2.0f);
    mStepCounter.init();
    mFloorCounter.init();
}

Service::~Service()
{
    disconnect();   // Cleanup recourses
}

void Service::run()
{
    LOG_INFO("Started\n");

    // Initialize time
    mTimeTracker.init();

    // Get settings
    if (!mSettingsSerializer.load(mSettings)) {
        LOG_WARNING("Failed to load settings\n");
    }

    // Get summary
    if (!mActivitySummarySerializer.load(mSummary)) {
        LOG_WARNING("Failed to load activity summary\n");
    }

    SDK::Timer guiInitTimeout(TIMER_SECONDS(5));
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
                        stopTrack(false);
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

                case CustomMessage::TRACK_PAUSE:  {
                    LOG_DEBUG("TRACK_PAUSE\n");
                    handleEvent(*static_cast<CustomMessage::TrackPause*>(msg));
                } break;

                case CustomMessage::TRACK_RESUME:  {
                    LOG_DEBUG("TRACK_RESUME\n");
                    handleEvent(*static_cast<CustomMessage::TrackResume*>(msg));
                } break;

                case CustomMessage::MANUAL_LAP:  {
                    LOG_DEBUG("MANUAL_LAP\n");
                    handleEvent(*static_cast<CustomMessage::ManualLap*>(msg));
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
            std::time_t utc = mTimeTracker.getExpectedUTC();

            if (processedUtc != utc) {

                processedUtc = utc;

                // Send to GUI real "local time" to display
                std::tm tmNow = mTimeTracker.getLocalTime(std::time(nullptr));
                mGuiSender.time(tmNow);

                mGuiSender.battery(static_cast<uint8_t>(mBatteryLevel.getLevel()));

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
                    mTimeCounter.add(utc);
                    processTrack();
                }
            }
        } else if (mGlanceActive) {
            // do nothing
        } else {
            // Just wait some time to see if GUI starts
            if (guiInitTimeout.expired()) {
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
        mSensorBatteryMetrics.connect();
        mSensorGpsSpeed.connect();
        mSensorGpsDistance.connect();
        mSensorStepCounter.connect();
        mSensorPressure.connect();
        mSensorFloorCounter.connect();
        mSensorHr.connect();

        mIsSensorsConnected = true;
    }
}

void Service::disconnect()
{
    if (mIsSensorsConnected) {
        LOG_DEBUG("Disconnect from sensors...\n");

        mSensorHr.disconnect();
        mSensorFloorCounter.disconnect();
        mSensorPressure.disconnect();
        mSensorStepCounter.disconnect();
        mSensorGpsSpeed.disconnect();
        mSensorGpsDistance.disconnect();
        mSensorBatteryLevel.disconnect();
        mSensorBatteryMetrics.disconnect();

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
                parser.getCoordinates(mGps.latitude, mGps.longitude, mGps.altitude);
            }
            LOG_DEBUG("Location: fix %u, lat %f, lon %f\n", mGps.fix, mGps.latitude, mGps.longitude);
        }
    } else if (mSensorGpsSpeed.matchesDriver(handle)) {
        SDK::SensorDataParser::GpsSpeed parser(data[0]);
        if (parser.isDataValid()) {
            mSpeedCounter.add(parser.getSpeed());
            LOG_DEBUG("Speed:    %.2f m/s\n", parser.getSpeed());
        }
    } else if (mSensorGpsDistance.matchesDriver(handle)) {
        SDK::SensorDataParser::GpsDistance parser(data[0]);
        if (parser.isDataValid()) {
            mDistanceCounter.add(parser.getDistance());
            LOG_DEBUG("Distance: %.2f m\n", parser.getDistance());
        }
    } else if (mSensorStepCounter.matchesDriver(handle)) {
        SDK::SensorDataParser::StepCounter parser(data[0]);
        if (parser.isDataValid()) {
            mStepCounter.add(parser.getStepCount());
            LOG_DEBUG("Steps %u\n", parser.getStepCount());
        }
    } else if (mSensorFloorCounter.matchesDriver(handle)) {
        SDK::SensorDataParser::FloorCounter parser(data[0]);
        if (parser.isDataValid()) {
            uint32_t newValue = parser.getFloorsUp() + parser.getFloorsDown();
            mFloorCounter.add(newValue);
            LOG_DEBUG("Floors %u (up: %u, down: %u)\n", newValue, parser.getFloorsUp(), parser.getFloorsDown());
        }
    } else if (mSensorPressure.matchesDriver(handle)) {
        SDK::SensorDataParser::Pressure parser(data[0]);
        if (parser.isDataValid()) {
            if (!mAltitudeCounter.isValid()) {
                // Save p0
                mSeaLevelPressure = parser.getP0();
            }
            float altitude = parser.getAltitude(parser.getPressure(), mSeaLevelPressure);
            float filtered = mAltitudeFilter.execute(altitude);
            mAltitudeCounter.add(filtered);

            LOG_DEBUG("Altitude %.2f (Filtered %.2f) (P0 %f, Pa %f)\n", altitude, filtered, mSeaLevelPressure, parser.getPressure());
        }
    } else if (mSensorHr.matchesDriver(handle)) {
        SDK::SensorDataParser::HeartRate parser(data[0]);
        if (parser.isDataValid()) {
            mHrCounter.add(parser.getBpm());
            mTrackData.hrTrustLevel = parser.getTrustLevel();
            LOG_DEBUG("HR %.1f, TrustLevel %.1f\n", parser.getBpm(), parser.getTrustLevel());
        }
    } else if (mSensorBatteryLevel.matchesDriver(handle)) {
        SDK::SensorDataParser::BatteryLevel parser(data[0]);
        if (parser.isDataValid()) {
            mBatteryLevel.setLevel(parser.getCharge());
            LOG_DEBUG("Battery %.1f %%\n", mBatteryLevel.getValue());
        }
    } else if (mSensorBatteryMetrics.matchesDriver(handle)) {
        SDK::SensorDataParser::BatteryMetrics parser(data[0]);
        if (parser.isDataValid()) {
            mBatteryLevel.setVoltage(parser.getVoltage());
            LOG_DEBUG("Battery voltage %.1f V\n", mBatteryLevel.getVoltage());
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
    // We can synchronize the time because we haven't started the track yet,
    // and the GPS could have already updated the current time.
    mTimeTracker.init();

    startTrack(mTimeTracker.getExpectedUTC());
}

void Service::handleEvent(const CustomMessage::TrackStop& event)
{
    stopTrack(event.discard);
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

void Service::handleEvent(const CustomMessage::TrackPause& event)
{
    pauseTrack(true);
}

void Service::handleEvent(const CustomMessage::TrackResume& event)
{
    pauseTrack(false); // resume
}

void Service::handleEvent(const CustomMessage::ManualLap& event)
{
    saveLap();
    mGuiSender.lapEnd(mTrackData.lapNum);
    notifyLapEnd();
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
        buzzerMsg->notes[0].time  = 150;
        buzzerMsg->notes[1].volume = 0;
        buzzerMsg->notes[1].time  = 100;
        buzzerMsg->notes[2].volume = 100;
        buzzerMsg->notes[2].time  = 150;
        buzzerMsg->notes[3].volume = 0;
        buzzerMsg->notes[3].time  = 100;
        buzzerMsg->notes[4].volume = 100;
        buzzerMsg->notes[4].time  = 150;
        buzzerMsg->notesCount = 5;

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
        buzzerMsg->notes[0].time  = 150;
        buzzerMsg->notes[1].volume = 0;
        buzzerMsg->notes[1].time  = 100;
        buzzerMsg->notes[2].volume = 100;
        buzzerMsg->notes[2].time  = 150;
        buzzerMsg->notes[3].volume = 0;
        buzzerMsg->notes[3].time  = 100;
        buzzerMsg->notes[4].volume = 100;
        buzzerMsg->notes[4].time  = 150;
        buzzerMsg->notesCount = 5;

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

ActivityWriter::RecordData Service::prepareRecordData()
{
    ActivityWriter::RecordData fitRecord{};

    fitRecord.timestamp    = mTimeCounter.getCurrent();

    fitRecord.set(ActivityWriter::RecordData::Field::COORDS, mGps.fix);
    fitRecord.latitude     = mGps.latitude;
    fitRecord.longitude    = mGps.longitude;

    fitRecord.set(ActivityWriter::RecordData::Field::SPEED, mSpeedCounter.isValid());
    fitRecord.speed        = mSpeedCounter.getCurrent();

    fitRecord.set(ActivityWriter::RecordData::Field::ALTITUDE, mAltitudeCounter.isValid());
    fitRecord.altitude     = mAltitudeCounter.getCurrent();

    bool hasHeartRate = (mHrCounter.getCurrent() > 20 && mTrackData.hrTrustLevel >= 1 && mTrackData.hrTrustLevel <= 3);
    fitRecord.set(ActivityWriter::RecordData::Field::HEART_RATE, hasHeartRate);
    fitRecord.heartRate    = mHrCounter.getCurrent();

    fitRecord.set(ActivityWriter::RecordData::Field::BATTERY, mBatteryLevel.readyToSave());
    fitRecord.batteryLevel   = static_cast<uint8_t>(mBatteryLevel.getLevel());
    fitRecord.batteryVoltage = static_cast<uint16_t>(mBatteryLevel.getVoltage() * 1000);

    return fitRecord;
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

    LOG_DEBUG("Settings: units %u, th: %u-%u-%u-%u\n", mUnits, hrThresholds[0], hrThresholds[1], hrThresholds[2], hrThresholds[3]);
    // Summary
    mGuiSender.summary(std::make_shared<const ActivitySummary>(mSummary));

    // Battery level
    mGuiSender.battery(static_cast<uint8_t>(mBatteryLevel.getLevel()));
}

void Service::startTrack(std::time_t utc)
{
    // Reset data
    memset(&mTrackData, 0, sizeof(mTrackData));

    mTimeCounter.reset();
    mTimeCounter.add(utc);

    mDistanceCounter.reset();
    mSpeedCounter.reset();
    mHrCounter.reset();
    mAltitudeFilter.reset();
    mAltitudeCounter.reset();
    mStepCounter.reset();
    mFloorCounter.reset();
    mBatteryLevel.reset();
    mBatteryLevel.setSaveRequest();
    mGps.reset();

    mSessionNotEmpty = false;
    mLapNotEmpty = false;

    // Configure TrackMapBuilder
    mTrackMapBuilder.reset();

    SDK::TrackMapBuilder::GpsPoint startGpsPoint{ mGps.latitude, mGps.longitude };
    mTrackMapBuilder.setDistanceThreshold(startGpsPoint, skMapDistanceThreshold);

    // Determinate Lap split source
    mLapDivSource = getLapDivSource();

    connectAll();

    ActivityWriter::AppInfo info{};
    info.timestamp = mTimeCounter.getCurrent();
    info.appVersion = SDK::ParseVersion(BUILD_VERSION).u32;
    info.devID = DEV_ID;
    info.appID = APP_ID;
    mActivityWriter.start(info);

    mTrackState = Track::State::ACTIVE;

    LOG_INFO("Track started. UTC: %u\n", static_cast<uint32_t>(mTimeCounter.getCurrent()));
    mGuiSender.trackState(mTrackState);
}

void Service::processTrack()
{
    LOG_DEBUG("Time: %u / %u\n", (uint32_t)mTimeCounter.getValueActive(), (uint32_t)mTimeCounter.getValueTotal());

    // GPS
    // Creating map
    SDK::TrackMapBuilder::GpsPoint newPoint{ mGps.latitude, mGps.longitude };
    // Add point to the track map
    if (mGps.fix) {
        mTrackMapBuilder.addPoint(newPoint);
    }

    // Collect data for GUI

    // Time, s
    mTrackData.totalTime = mTimeCounter.getValueActive();
    mTrackData.lapTime = mTimeCounter.getLapValueActive();

    // Distance, m
    mTrackData.distance = mDistanceCounter.getValueActive();
    mTrackData.lapDistance = mDistanceCounter.getLapValueActive();

    // Speed, m/s
    mTrackData.speed = mSpeedCounter.getCurrent();

#if 1
    mTrackData.avgSpeed = mSpeedCounter.getAverage();
#else
    mTrackData.avgSpeed = mDistanceCounter.getValueActive() / mTimeCounter.getValueActive();
#endif
    mTrackData.maxSpeed = mSpeedCounter.getMaximum();

#if 1
    mTrackData.avgLapSpeed = mSpeedCounter.getLapAverage();
#else
    mTrackData.avgLapSpeed = mDistanceCounter.getLapValueActive() / mTimeCounter.getLapValueActive();
#endif
    mTrackData.maxLapSpeed = mSpeedCounter.getLapMaximum();

    // Pace, s/m
    const float kMinSpeed = mSpeedCounter.getMinValid();
    mTrackData.pace = getPace(mTrackData.speed, kMinSpeed);
    mTrackData.avgPace = getPace(mTrackData.avgSpeed, kMinSpeed);
    mTrackData.lapPace = getPace(mTrackData.avgLapSpeed, kMinSpeed);

    // HR
    mTrackData.HR = mHrCounter.getCurrent();
    mTrackData.avgHR = mHrCounter.getAverage();
    mTrackData.maxHR = mHrCounter.getMaximum();
    mTrackData.avgLapHR = mHrCounter.getLapAverage();
    mTrackData.maxLapHR = mHrCounter.getLapMaximum();

    // Altitude, m
    mTrackData.elevation = mAltitudeCounter.getCurrent();

    // Steps
    mTrackData.steps = mStepCounter.getValueActive();
    mTrackData.lapSteps = mStepCounter.getLapValueActive();

    // Floors
    mTrackData.floors = mFloorCounter.getValueActive();;
    mTrackData.lapFloors = mFloorCounter.getLapValueActive();

    // Update GUI
    mGuiSender.trackData(mTrackData);

    if (mTrackState == Track::State::ACTIVE) {
        // Save record to the FIT file
        ActivityWriter::RecordData fitRecord = prepareRecordData();
        mActivityWriter.addRecord(fitRecord);

        mSessionNotEmpty = true;    // Session has at least one record
        mLapNotEmpty = true;        // Lap has at least one record

        // Next lap
        bool switchLap = false;
        switch (mLapDivSource) {
        case LapDivSource::STEPS:
            switchLap = mStepCounter.getLapValueActive() >= mSettings.alertSteps;
            break;
        case LapDivSource::DISTANCE:
            switchLap = mDistanceCounter.getLapValueActive() >= (mSettings.alertDistance * 1000.0f);
            break;
        case LapDivSource::TIME:
            switchLap = mTimeCounter.getLapValueActive() >= (mSettings.alertTime * 60);
            break;

        case LapDivSource::OFF:
        default:
            break;
        }

        if (switchLap) {
            saveLap();
            mGuiSender.lapEnd(mTrackData.lapNum);
            notifyLapEnd();
        }
    }
}

void Service::saveLap()
{
    // Save lap to the FIT file
    ActivityWriter::LapData fitLap{};

    fitLap.timestamp = mTimeCounter.getCurrent();
    fitLap.timeStart = mTimeCounter.getCurrent() - mTimeCounter.getLapValueTotal();
    fitLap.duration  = mTimeCounter.getLapValueActive();
    fitLap.elapsed   = mTimeCounter.getLapValueTotal();

    fitLap.distance  = mDistanceCounter.getLapValueActive();

    fitLap.speedAvg  = mSpeedCounter.getLapAverage();
    fitLap.speedMax  = mSpeedCounter.getLapMaximum();

    fitLap.steps     = mStepCounter.getLapValueActive();

    fitLap.floors    = mFloorCounter.getLapValueActive();

    fitLap.hrAvg     = mHrCounter.getLapAverage();
    fitLap.hrMax     = mHrCounter.getLapMaximum();

    fitLap.ascent    = mAltitudeCounter.getLapAscent();
    fitLap.descent   = mAltitudeCounter.getLapDescent();

    mActivityWriter.addLap(fitLap);
    mTrackData.lapNum++;

    LOG_INFO("Lap_%u saved. UTC: %u\n", mTrackData.lapNum, static_cast<uint32_t>(mTimeCounter.getCurrent()));
    LOG_INFO("Time: %u / %u s\n", static_cast<uint32_t>(mTimeCounter.getLapValueActive()), static_cast<uint32_t>(mTimeCounter.getLapValueTotal()));
    LOG_INFO("Distance: %.3f m\n", mDistanceCounter.getLapValueActive());
    LOG_INFO("Speed: %.3f / %.3f m/s\n", mSpeedCounter.getLapAverage(), mSpeedCounter.getLapMaximum());
    LOG_INFO("Heart rate: %.0f / %.0f bpm\n", mHrCounter.getLapAverage(), mHrCounter.getLapMaximum());
    LOG_INFO("Ascent/Descent: %.1f / %.1f m\n", mAltitudeCounter.getLapAscent(), mAltitudeCounter.getLapDescent());
    LOG_INFO("Steps: %u\n", mStepCounter.getLapValueActive());
    LOG_INFO("Floors: %u\n", mFloorCounter.getLapValueActive());


    // Reset lap counters
    mTimeCounter.resetLap();
    mDistanceCounter.resetLap();
    mSpeedCounter.resetLap();
    mHrCounter.resetLap();
    mAltitudeCounter.resetLap();
    mStepCounter.resetLap();
    mFloorCounter.resetLap();

    // Clear track data
    mTrackData.lapTime = 0;
    mTrackData.lapDistance = 0.0f;
    mTrackData.lapSteps = 0;
    mTrackData.lapFloors = 0;
    mTrackData.lapElevation = 0.0f;
    mTrackData.maxLapSpeed = 0.0f;
    mTrackData.avgLapSpeed = 0.0f;
    mTrackData.avgLapHR = 0.0f;
    mTrackData.maxLapHR = 0.0f;

    mLapNotEmpty = false;
}

void Service::stopTrack(bool discard)
{
    if (mTrackState == Track::State::INACTIVE) {
        return;
    }

    if (!discard && mSessionNotEmpty) {

        if (mTrackState != Track::State::PAUSED) {
            mActivityWriter.pause(mTimeCounter.getCurrent());
        }

        if (mLapNotEmpty) {
            saveLap();
        }

        mBatteryLevel.setSaveRequest();
        ActivityWriter::RecordData fitRecord = prepareRecordData();
        mActivityWriter.addRecord(fitRecord);

        mSummary.utc       = mTimeCounter.getCurrent();
        mSummary.time      = mTimeCounter.getValueActive();
        mSummary.distance  = mDistanceCounter.getValueActive();
        mSummary.speedAvg  = mSpeedCounter.getAverage();
        mSummary.steps     = mStepCounter.getValueActive();
        mSummary.elevation = mAltitudeCounter.getCurrent();
        mSummary.paceAvg   = getPace(mSpeedCounter.getAverage(), mSpeedCounter.getMinValid());
        mSummary.hrMax     = mHrCounter.getMaximum();
        mSummary.hrAvg     = mHrCounter.getAverage();
        mSummary.map       = mTrackMapBuilder.build(70);

        // Save summary
        if (!mActivitySummarySerializer.save(mSummary)) {
            LOG_ERROR("Can't save activity summary\n");
        }
        mGuiSender.summary(std::make_shared<const ActivitySummary>(mSummary));

        // Save FIT file
        ActivityWriter::TrackData fitTrack{};

        fitTrack.timestamp = mTimeCounter.getCurrent();
        fitTrack.timeStart = mTimeCounter.getCurrent() - mTimeCounter.getValueTotal();
        fitTrack.duration  = mTimeCounter.getValueActive();
        fitTrack.elapsed   = mTimeCounter.getValueTotal();

        fitTrack.distance  = mDistanceCounter.getValueActive();    // m

        fitTrack.speedAvg  = mSpeedCounter.getAverage();    // m/s
        fitTrack.speedMax  = mSpeedCounter.getMaximum();    // m/s

        fitTrack.hrAvg     = mHrCounter.getAverage();
        fitTrack.hrMax     = mHrCounter.getMaximum();

        fitTrack.ascent    = mAltitudeCounter.getAscent();
        fitTrack.descent   = mAltitudeCounter.getDescent();

        fitTrack.steps     = mStepCounter.getValueActive();

        fitTrack.floors    = mFloorCounter.getValueActive();

        mActivityWriter.stop(fitTrack);

        notifyNewActivity();
    } else {
        mActivityWriter.discard();
    }

    mTrackState = Track::State::INACTIVE;
    LOG_INFO("Track stopped. UTC: %u\n", static_cast<uint32_t>(mTimeCounter.getCurrent()));
    LOG_INFO("Time: %u / %u s\n", static_cast<uint32_t>(mTimeCounter.getValueActive()), static_cast<uint32_t>(mTimeCounter.getValueTotal()));
    LOG_INFO("Distance: %.3f m\n", mDistanceCounter.getValueActive());
    LOG_INFO("Speed: %.3f / %.3f m/s\n", mSpeedCounter.getAverage(), mSpeedCounter.getMaximum());
    LOG_INFO("Heart rate: %.0f / %.0f bpm\n", mHrCounter.getAverage(), mHrCounter.getMaximum());
    LOG_INFO("Ascent/Descent: %.1f / %.1f m\n", mAltitudeCounter.getAscent(), mAltitudeCounter.getDescent());
    LOG_INFO("Steps: %u\n", mStepCounter.getValueActive());
    LOG_INFO("Floors: %u\n", mFloorCounter.getValueActive());

    mGuiSender.trackState(mTrackState);

    disconnect();

    mBatteryLevel.timer.stop();
}

void Service::pauseTrack(bool pause)
{
    if (mTrackState == Track::State::INACTIVE) {
        return;
    }

    if (pause && mTrackState == Track::State::ACTIVE) {
        mTimeCounter.pause();
        mDistanceCounter.pause();
        mSpeedCounter.pause();
        mHrCounter.pause();
        mAltitudeCounter.pause();
        mStepCounter.pause();
        mFloorCounter.pause();

        mActivityWriter.pause(mTimeCounter.getCurrent());

        mTrackState = Track::State::PAUSED;
        LOG_INFO("Track paused. UTC: %u\n", static_cast<uint32_t>(mTimeCounter.getCurrent()));
        mGuiSender.trackState(mTrackState);
    } else if (!pause && mTrackState == Track::State::PAUSED) {
        mTimeCounter.resume();
        mDistanceCounter.resume();
        mSpeedCounter.resume();
        mHrCounter.resume();
        mAltitudeCounter.resume();
        mStepCounter.resume();
        mFloorCounter.resume();

        mActivityWriter.resume(mTimeCounter.getCurrent());

        mTrackState = Track::State::ACTIVE;
        LOG_INFO("Track resumed. UTC: %u\n", static_cast<uint32_t>(mTimeCounter.getCurrent()));
        mGuiSender.trackState(mTrackState);
    }
}

Service::LapDivSource Service::getLapDivSource()
{
    size_t stepId = App::Menu::RoundToNearestIndex(App::Menu::kStepsList,
        App::Menu::Settings::Alerts::Steps::ID_COUNT, static_cast<float>(mSettings.alertSteps));

    float distance = mUnits ? App::Utils::km2mi(mSettings.alertDistance) : mSettings.alertDistance;
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
