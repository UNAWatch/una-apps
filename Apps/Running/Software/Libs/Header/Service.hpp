#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

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
    SDK::Kernel&          mKernel;
    bool                  mGUIStarted;
    CustomMessage::Sender mGuiSender;

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

    void handleSensorsData(uint16_t handle, SDK::Sensor::DataBatch& data);

    // User-defined event handlers
    void handleEvent(const CustomMessage::TrackStart& event);
    void handleEvent(const CustomMessage::TrackStop& event);
    void handleEvent(const CustomMessage::SettingsUpd& event);

    void setCapabilities();
    void notifyFirstFix();
    void notifyLapEnd();
    void notifyNewActivity();

    // Time management

    /// UTC time at the start of the app, in seconds
    std::time_t mStartUTC;

    /// Local time offset at the start of the app, in seconds
    std::time_t mStartLocalTimeOffset;

    /// Monotonic time at the start of the app, in milliseconds
    uint32_t mStartMono;

    /**
     * @brief Initialize starting timestamps for monotonic tracking.
     */
    void initTime();

    /**
     * @brief Get the expected UTC based on initial timestamp and elapsed time.
     * @note Monotonically increasing; unaffected by RTC or timezone changes.
     * @return Expected UTC time.
     */
    std::time_t getExpectedUTC();

    /**
     * @brief Get the expected local time based on initial timestamp and elapsed time.
     * @param utc Expected UTC time.
     * @note Monotonically increasing; unaffected by RTC or timezone changes.
     * @return Corresponding local time.
     */
    std::tm getExpectedLocalTime(std::time_t utc);

    /**
     * @brief Get the current system local time for a given UTC.
     * @param utc Reference UTC time.
     * @note Reflects real-time changes; affected by RTC and timezone.
     * @return Corresponding system local time.
     */
    std::tm getLocalTime(std::time_t utc);


    // Sensors
    SDK::Sensor::Connection mSensorGpsLocation;
    SDK::Sensor::Connection mSensorGpsSpeed;
    SDK::Sensor::Connection mSensorGpsDistance;
    SDK::Sensor::Connection mSensorPressure;
    SDK::Sensor::Connection mSensorHr;
    SDK::Sensor::Connection mSensorBatteryLevel;
    SDK::Sensor::Connection mSensorWristMotion;

    bool mIsSensorsConnected = false;

    static constexpr uint32_t skBacklightTimeout    = 5000;
    static constexpr uint32_t skInitialSamplePeriod = 1000;
    static constexpr uint32_t skSamplePeriod        = 1000;
    static constexpr uint32_t skSampleLatency       = 1000;

    static constexpr float skMapDistanceThreshold = 10.0f; // meters

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
    } mSpeed {};

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
        float    p0;        // sea-level pressure, Pa
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
            p0 = 0.0f;
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

#endif  // __SERVICE_HPP__
