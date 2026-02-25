
#pragma once

#include "SDK/Kernel/Kernel.hpp"

#include "SDK/SensorLayer/SensorConnection.hpp"
#include "SDK/SensorLayer/SensorDataBatch.hpp"

#include "SDK/TrackMap/TrackMapBuilder.hpp"
#include "SDK/Glance/GlanceControl.hpp"

#include "SettingsSerializer.hpp"
#include "ActivitySummarySerializer.hpp"
#include "ActivityWriter.hpp"

#include "MonotonicTime.hpp"
#include "MonotonicCounter.hpp"
#include "VariableCounter.hpp"
#include "DeltaCounter.hpp"
#include "SimpleLPF.hpp"
#include "SDK/Timer/Timer.hpp"

#include "Commands.hpp"

class Service
{
public:
    Service(SDK::Kernel &kernel);

    virtual ~Service();

    void run();

private:
    SDK::Kernel&            mKernel;
    bool                    mGUIStarted;
    CustomMessage::Sender   mGuiSender;

    Settings mSettings;
    bool mUnits = false;
    SettingsSerializer mSettingsSerializer;

    ActivitySummary mSummary;
    ActivitySummarySerializer mActivitySummarySerializer;
    ActivityWriter mActivityWriter;
    SDK::TrackMapBuilder mTrackMapBuilder;


    void connectGps();
    void connectAll(); // Except GPS
    void disconnect();
    void onStartGUI();
    void onStopGUI();

    void handleSensorsData(uint16_t handle, SDK::Sensor::DataBatch& data);

    // User-defined event handlers
    void handleEvent(const CustomMessage::TrackStart& event);
    void handleEvent(const CustomMessage::TrackStop& event);
    void handleEvent(const CustomMessage::SettingsUpd& event);
    void handleEvent(const CustomMessage::TrackPause& event);
    void handleEvent(const CustomMessage::TrackResume& event);
    void handleEvent(const CustomMessage::ManualLap& event);

    void setCapabilities();
    void notifyFirstFix();
    void notifyLapEnd();
    void notifyNewActivity();




    // Sensors
    SDK::Sensor::Connection mSensorGpsLocation;
    SDK::Sensor::Connection mSensorGpsSpeed;
    SDK::Sensor::Connection mSensorGpsDistance;
    SDK::Sensor::Connection mSensorStepCounter;
    SDK::Sensor::Connection mSensorFloorCounter;
    SDK::Sensor::Connection mSensorPressure;
    SDK::Sensor::Connection mSensorHr;
    SDK::Sensor::Connection mSensorBatteryLevel;
    SDK::Sensor::Connection mSensorWristMotion;
    bool mIsSensorsConnected = false;

    static constexpr uint32_t skBacklightTimeout    = 5000;
    static constexpr uint32_t skInitialSamplePeriod = 1000;
    static constexpr uint32_t skSamplePeriod        = 1000;
    static constexpr uint32_t skSampleLatency       = 1000;

    static constexpr float skMapDistanceThreshold   = 10.0f; // meters

    SDK::MonotonicTime                  mTimeTracker;
    SDK::MonotonicCounter<std::time_t>  mTimeCounter;
    SDK::MonotonicCounter<float>        mDistanceCounter;
    SDK::VariableCounter                mSpeedCounter;
    SDK::VariableCounter                mHrCounter;
    SDK::Filter::SimpleLPF              mAltitudeFilter;
    SDK::DeltaCounter                   mAltitudeCounter;
    SDK::MonotonicCounter<uint32_t>     mStepCounter;
    SDK::MonotonicCounter<uint32_t>     mFloorCounter;

    // GPS info
    struct {
        bool     gotFix;        // fix received at least once
        bool     fix;           // Actual GPS fix
        float    latitude;      // degrees
        float    longitude;     // degrees
        float    altitude;      // meters
        uint32_t timestamp;     // ms

        void reset()
        {
            gotFix    = false;
            fix       = false;
            latitude  = 0.0f;
            longitude = 0.0f;
            altitude  = 0.0f;
            timestamp = 0;
        }
    } mGps{};

    // Sea-level pressure, Pa
    float mSeaLevelPressure = 0.0f;

    // Current battery level
    struct {
        SDK::Timer timer;
        float      soc;
        bool       isValid;
        bool       saveRequest;

        void setValue(float v)
        {
            soc   = v;
            isValid = true;
        }

        float getValue()
        {
            return soc;
        }

        void setSaveRequest()
        {
            saveRequest = true;
        }

        bool readyToSave()
        {
            if (!isValid) {
                return false;
            }

            if (timer.tick()) {
                saveRequest = true;
            }

            if (!saveRequest) {
                return false;
            }

            saveRequest = false;

            return true;
        }

        void reset()
        {
            timer.start(TIMER_MINUTES(5));
            soc         = 0.0f;
            isValid     = false;
            saveRequest = true;
        }
    } mBatteryLevel{};

    Track::State mTrackState = Track::State::INACTIVE;
    std::time_t  mTrackStartUTC = 0;
    bool         mPreviousGpsFixState = false;
    std::time_t  mTrackProcessTimestamp = 0;

    bool mSessionNotEmpty = false;
    bool mLapNotEmpty = false;

    Track::Data mTrackData{};

    enum class LapDivSource {
        OFF = 0,
        STEPS,
        DISTANCE,
        TIME,
    };

    LapDivSource mLapDivSource{};

    ActivityWriter::RecordData prepareRecordData();

    void sendInitialInfoToGui();
    void startTrack(std::time_t utc);
    void processTrack();
    void saveLap();
    void stopTrack(bool discard);
    void pauseTrack(bool pause);
    LapDivSource getLapDivSource();

    inline float getPace(float speed, float th)
    {
        return (speed > th) ? (1.0f / speed) : 0.0f;
    }

    // IGlance implementation
    bool                     mGlanceActive = false;
    const char*              mName = nullptr;
    uint32_t                 mMaxControls = 0;
    SDK::Glance::Form        mGlanceUI {};
    SDK::Glance::ControlText mGlanceTitle {};
    SDK::Glance::ControlText mGlanceTime {};

    void onGlanceTick();
    bool configGui();
    void createGuiControls();
};

