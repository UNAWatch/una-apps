
#pragma once

#include "SDK/Kernel/Kernel.hpp"

#include "SDK/SensorLayer/SensorConnection.hpp"
#include "SDK/SensorLayer/SensorDataBatch.hpp"

#include "SDK/TrackMap/TrackMapBuilder.hpp"
#include "SDK/Glance/GlanceControl.hpp"

#include "SettingsSerializer.hpp"
#include "ActivitySummarySerializer.hpp"
#include "ActivityWriter.hpp"

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
    bool mGotFix;

    void connectGps();
    void connectAll(); // Except GPS
    void disconnect();
    void onStartGUI();
    void onStopGUI();

    // ISensorDataListener implementation
    void onSdlNewData(uint16_t handle, SDK::Sensor::DataBatch& data);

    // User-defined event handlers
    void handleEvent(const CustomMessage::TrackStart& event);
    void handleEvent(const CustomMessage::TrackStop& event);
    void handleEvent(const CustomMessage::SettingsUpd& event);

    void setCapabilities();
    void notifyFirstFix();
    void notifyLapEnd();
    void notifyNewActivity();
    std::tm toLocalTime(std::time_t utc);

    // Sensors
    SDK::Sensor::Connection mSensorGpsLocation;
    SDK::Sensor::Connection mSensorGpsSpeed;
    SDK::Sensor::Connection mSensorGpsDistance;
    SDK::Sensor::Connection mSensorStepCounter;
    SDK::Sensor::Connection mSensorFloorCounter;
    SDK::Sensor::Connection mSensorAltimeter;
    SDK::Sensor::Connection mSensorHr;
    SDK::Sensor::Connection mSensorBatteryLevel;
    SDK::Sensor::Connection mSensorWristMotion;
    bool mIsSensorsConnected = false;

    static constexpr uint32_t skBacklightTimeout    = 5000;
    static constexpr uint32_t skInitialSamplePeriod = 1000;
    static constexpr uint32_t skSamplePeriod        = 10000;
    static constexpr uint32_t skSampleLatency       = 1000;

    static constexpr float skMapDistanceThreshold   = 10.0f; // meters

    struct {
        // Last sensor data
        bool     fix;           // GPS fix
        float    latitude;      // degrees
        float    longitude;     // degrees
        float    altitude;      // meters
        uint32_t timestamp;     // ms

        void reset()
        {
            fix       = false;
            latitude  = 0.0f;
            longitude = 0.0f;
            altitude  = 0.0f;
            timestamp = 0;
        }
    } mGps{};

    struct {
        float    speed;     // m/s
        uint32_t timestamp; // ms

        void reset() {
            speed     = 0.0f;
            timestamp = 0;
        }
    } mSpeed{};

    struct {
        float    distance;  // m
        uint32_t timestamp; // ms

        // Working data
        bool     dataValid;
        float    initialDistance; // m

        void reset() {
            distance  = 0.0f;
            timestamp = 0;
            dataValid = false;
            initialDistance = 0.0f;
        }
    } mDistance{};

    struct {
        // Last sensor data
        uint32_t steps;     // steps
        uint32_t timestamp; // ms

        // Working data
        bool dataValid;
        uint32_t initialSteps;

        void reset()
        {
            steps = 0;
            timestamp = 0;
            dataValid = false;
            initialSteps = 0;
        }
    } mStepCounter{};

    struct {
        // Last sensor data
        uint32_t floors;     // floors
        uint32_t timestamp;  // ms

        // Working data
        bool dataValid;
        uint32_t initialFloors;

        void reset()
        {
            floors = 0;
            timestamp = 0;
            dataValid = false;
            initialFloors = 0;
        }
    } mFloorsCounter{};

    struct {
        // Last sensor data
        float    altitude;  // meters
        uint32_t timestamp; // ms

        // Working data
        bool dataValid;
        float initialAltitude;

        float ascent;           // m
        float descent;          // m

        float lapAscent;        // m
        float lapDescent;       // m

        void reset()
        {
            altitude = 0.0f;
            timestamp = 0;
            dataValid = false;
            initialAltitude = 0.0f;
            ascent = 0.0f;
            descent = 0.0f;
            lapAscent = 0.0f;
            lapDescent = 0.0f;
        }
    } mAltimeter{};

    struct {
        // Last sensor data
        float  hr;        // bpm
        float  trustLevel; // trust level: 0, 1, 2, 3...
        uint32_t timestamp; // ms

        // Working data
        float    totalSum;
        uint32_t totalCnt;

        float    lapSum;
        uint32_t lapCnt;

        void reset()
        {
            hr = 0.0f;
            timestamp = 0;
            totalSum = 0.0f;
            totalCnt = 0;
            lapSum = 0.0f;
            lapCnt = 0;
        }
    } mHr{};

    struct {
        float level = 0.0f;
    } mBattery{};

    Track::State mTrackState = Track::State::INACTIVE;
    std::time_t mTrackStartUTC = 0;
    bool mPreviousGpsFixState = false;
    std::time_t mTrackProcessTimestamp = 0;

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

    void sendInitialInfoToGui();
    void startTrack(std::time_t utc);
    void processTrack(std::time_t utc);
    void saveLap(std::time_t utc);
    void stopTrack(std::time_t utc, bool discard);
    LapDivSource getLapDivSource();
    uint32_t getCurrentLap();


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

