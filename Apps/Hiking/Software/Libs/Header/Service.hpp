#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include "SDK/GSModel/GSModelHelper.hpp"

#include "SDK/Interfaces/ISensorDriver.hpp"
#include "SDK/Interfaces/ISensorDataListener.hpp"
#include "SDK/TrackMap/TrackMapBuilder.hpp"

#include "SettingsSerializer.hpp"
#include "ActivitySummarySerializer.hpp"
#include "ActivityWriter.hpp"


class Service : public IServiceModelHandler,
    public SDK::Interface::IUserApp::Callback,
                public SDK::Interface::ISensorDataListener
{
public:
    Service(const IKernel& kernel);

    virtual ~Service() = default;

    void run();


private:
    virtual void onCreate()  override;
    virtual void onStart()   override;
    virtual void onResume()  override;
    virtual void onStop()    override;
    virtual void onPause()   override;
    virtual void onDestroy() override;

    virtual void sdlNewData(const SDK::Interface::ISensorDriver* sensor,
        const std::vector<SDK::Interface::ISensorData*>& data, bool first) override;

    virtual void handleEvent(const G2SEvent::GuiRun& event) override;
    virtual void handleEvent(const G2SEvent::GuiStop& event) override;
    virtual void handleEvent(const G2SEvent::TrackStart& event) override;
    virtual void handleEvent(const G2SEvent::TrackStop& event) override;
    virtual void handleEvent(const G2SEvent::SettingsSave& event) override;
    virtual void handleEvent(const G2SEvent::InternalRefresh& event) override;


    // Force refresh service state
    void refreshService();

    void sendInitialInfoToGui();



    const IKernel&                  mKernel;
    std::shared_ptr<GSModelService> mGSModel;
    bool                            mTerminate;
    bool                            mGUIStarted;

    Settings mSettings;
    SettingsSerializer mSettingsSerializer;
    
    ActivitySummary mSummary;
    ActivitySummarySerializer mActivitySummarySerializer;
    SDK::TrackMapBuilder mTrackMapBuilder;
    ActivityWriter mActivityWriter;

    // Sensors
    SDK::Interface::ISensorDriver* mGpsSensor = nullptr;
    SDK::Interface::ISensorDriver* mStepCounterSensor = nullptr;
    SDK::Interface::ISensorDriver* mFloorCounterSensor = nullptr;
    SDK::Interface::ISensorDriver* mAltimeterSensor = nullptr;
    SDK::Interface::ISensorDriver* mHrSensor = nullptr;

    static constexpr uint32_t skInitialSamplePeriod = 1000;
    static constexpr uint32_t skSamplePeriod        = 10000;
    static constexpr uint32_t skSampleLatency       = 1000;

    static constexpr float skMapDistanceThreshold = 10.0f; // meters

    struct {
        // Last sensor data
        bool  fix;          // GPS fix
        float latitude;     // degrees
        float longitude;    // degrees
        float altitude;     // meters
        uint32_t timestamp; // ms
    } mGps {};

    struct {
        // Last sensor data
        uint32_t steps;     // steps
        uint32_t timestamp; // ms

        // Working data
        uint32_t initialSteps = 0; // 0 - not avaliable
    } mStepCounter {};

    struct {
        // Last sensor data
        uint32_t floors;     // floors
        uint32_t timestamp; // ms

        // Working data
        uint32_t initialFloors = 0; // 0 - not avaliable
    } mFloorsCounter {};

    struct {
        // Last sensor data
        float    altitude;  // meters
        uint32_t timestamp; // ms

        // Working data
        float initialAltitude = 0.0f; // 0 - not avaliable

        float ascent;           // m
        float descent;          // m

        float lapAscent;        // m
        float lapDescent;       // m
    } mAltimeter {};
    
    struct {
        // Last sensor data
        uint8_t  hr;        // bpm
        uint32_t timestamp; // ms

        // Working data
        float    totalSum;
        uint32_t totalCnt;

        float    lapSum;
        uint32_t lapCnt;
    } mHr {};

    Track::State mTrackState = Track::State::INACTIVE;
    std::time_t mTrackStartUTC = 0;
    bool mPreviousGpsFixState = false;
    std::time_t mTrackProcessTimestamp = 0;
    SDK::TrackMapBuilder::GpsPoint mPrevGpsPoint {};

    Track::Data mTrackData {};

    void startTrack();
    void processTrack(std::time_t utc);
    void stopTrack(bool discard);

    enum class LapDivSource {
        OFF = 0,
        STEPS,
        DISTANCE,
        TIME,
    };
    LapDivSource getLapDivSource();
    uint32_t getCurrentLap();
    static uint32_t ParseVersion(const char* str);

};

#endif  // __SERVICE_HPP__
