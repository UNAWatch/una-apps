
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
    /**
     * @brief Battery state-of-charge (SoC) sampling and throttled FIT logging helper.
     *
     * Keeps the latest SoC value and decides when it should be written to the FIT file:
     * - periodically (every 5 minutes), and/or
     * - on demand via a forced save request.
     *
     * The first valid value is written immediately after @ref reset() because @ref saveRequest
     * is set to true there.
     */
    struct
    {
        /** @brief Periodic save timer (5 minutes by default). */
        SDK::Timer timer;

        /** @brief Last known state of charge value, in percent. */
        float soc;

        /** @brief Indicates that @ref soc contains a valid value. */
        bool isValid;

        /**
         * @brief Indicates a pending forced save request.
         *
         * When set, the next call to @ref readyToSave() will return true (if the value is valid),
         * and this flag will be cleared.
         *
         * Mark battery level to be written with the next record (forced save).
         */
        bool saveRequest;

        /**
         * @brief Update the stored SoC value.
         * @param v State of charge, in percent.
         */
        void setValue(float v)
        {
            soc     = v;
            isValid = true;
        }

        /**
         * @brief Get the stored SoC value.
         * @return State of charge, in percent.
         */
        float getValue()
        {
            return soc;
        }

        /**
         * @brief Request a forced SoC write.
         *
         * Sets @ref saveRequest so that the next record can include the battery field,
         * bypassing the periodic interval if needed.
         */
        void setSaveRequest()
        {
            saveRequest = true;
        }

        /**
         * @brief Check whether SoC should be written now.
         *
         * Returns true when:
         * - a valid value is available, and
         * - either the periodic timer has elapsed or a forced save was requested.
         *
         * When it returns true, the internal save request flag is cleared.
         *
         * @return True if SoC should be written with the current/next record.
         */
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

        /**
         * @brief Reset internal state and start periodic saving.
         *
         * Starts a 5-minute timer, clears validity, and requests an initial save.
         */
        void reset()
        {
            timer.start(TIMER_MINUTES(5));
            soc         = 0.0f;
            isValid     = false;
            saveRequest = false;
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

