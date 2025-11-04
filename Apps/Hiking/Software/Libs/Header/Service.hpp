#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include "SDK/GSModel/GSModel.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"

#include "SDK/Interfaces/ISensorDriver.hpp"
#include "SDK/Interfaces/ISensorDataListener.hpp"
#include "SDK/TrackMap/TrackMapBuilder.hpp"
#include "SDK/Interfaces/IGlance.hpp"
#include "SDK/Glance/GlanceControl.hpp"
#include "SDK/SensorLayer/SensorDriverConnection.hpp"

#include "SettingsSerializer.hpp"
#include "ActivitySummarySerializer.hpp"
#include "ActivityWriter.hpp"


class Service : public IServiceModelHandler,
                public SDK::Interface::IApp::Callback,
                public SDK::Interface::ISensorDataListener,
                public SDK::Interface::IGlance
{
public:
    Service();

    virtual ~Service() = default;

    void run();

private:
    const SDK::Kernel&  mKernel;
    GSModel             mGSModel;

    bool                mTerminate;
    bool                mGUIStarted;

    // IApp::Callback implementation
    virtual void onStart()   override;
    virtual void onStop()    override;
    virtual void onStartGUI() override;
    virtual void onStopGUI()  override;

    // ISensorDataListener implementation
    virtual void onSdlNewData(const SDK::Interface::ISensorDriver*             sensor,
                              const std::vector<SDK::Interface::ISensorData*>& data,
                              bool                                             first) override;

    // User-defined event handlers
    virtual void handleEvent(const G2SEvent::TrackStart& event) override;
    virtual void handleEvent(const G2SEvent::TrackStop& event) override;
    virtual void handleEvent(const G2SEvent::SettingsSave& event) override;

    Settings mSettings;
    SettingsSerializer mSettingsSerializer;
    
    ActivitySummary mSummary;
    ActivitySummarySerializer mActivitySummarySerializer;
    SDK::TrackMapBuilder mTrackMapBuilder;
    ActivityWriter mActivityWriter;

    // Sensors
    SDK::Sensor::DriverConnection mGpsLocationSensor;
    SDK::Sensor::DriverConnection mGpsSpeedSensor;
    SDK::Sensor::DriverConnection mGpsDistanceSensor;
    SDK::Sensor::DriverConnection mStepCounterSensor;
    SDK::Sensor::DriverConnection mFloorCounterSensor;
    SDK::Sensor::DriverConnection mAltimeterSensor;
    SDK::Sensor::DriverConnection mHrSensor;

    static constexpr uint32_t skInitialSamplePeriod = 1000;
    static constexpr uint32_t skSamplePeriod        = 10000;
    static constexpr uint32_t skSampleLatency       = 1000;

    static constexpr float skMapDistanceThreshold = 10.0f; // meters

    struct {
        // Last sensor data
        bool     fix;           // GPS fix
        float    latitude;      // degrees
        float    longitude;     // degrees
        float    altitude;      // meters
        uint32_t timestamp;     // ms
        float    speed;         // m/s
        float    distance;      // meters

        void reset()
        {
            fix       = false;
            latitude  = 0.0f;
            longitude = 0.0f;
            altitude  = 0.0f;
            timestamp = 0;
            speed     = 0.0f;
            distance  = 0.0f;
        }
    } mGps{};

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

    Track::State mTrackState = Track::State::INACTIVE;
    std::time_t mTrackStartUTC = 0;
    bool mPreviousGpsFixState = false;
    std::time_t mTrackProcessTimestamp = 0;
    SDK::TrackMapBuilder::GpsPoint mPrevGpsPoint{};

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
    void startTrack();
    void processTrack(std::time_t utc);
    void saveLap(std::time_t utc);
    void stopTrack(bool discard);
    LapDivSource getLapDivSource();
    uint32_t getCurrentLap();
    static uint32_t ParseVersion(const char* str);


    // IGlance implementation
    virtual IGlance::Info glanceGetInfo() override;
    virtual void glanceUpdate()           override;
    virtual void glanceClose()            override;
    void createGlanceGUI();

    bool mGlanceActive = false;
    SDK::Glance::Form mGlanceUI;
    SDK::Glance::ControlText mGlanceTitle;
    SDK::Glance::ControlText mGlanceTime;
};

#endif  // __SERVICE_HPP__
