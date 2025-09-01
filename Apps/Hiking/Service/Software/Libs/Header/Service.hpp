#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include "Interfaces/IKernel.hpp"
#include "Service/Software/Libs/Header/GSModel.hpp"
#include "Service/Software/Libs/Header/SettingsSerializer.hpp"
#include "Service/Software/Libs/Header/ActivitySummarySerializer.hpp"
#include "Service/Software/Libs/Header/ActivityWriter.hpp"

#include "SensorLayer/ISensorDriver.hpp"
#include "SensorLayer/ISensorDataListener.hpp"
#include "TrackMapBuilder.hpp"

class Service : public IServiceModelHandler,
                public Interface::IUserApp::Callback,
                public Interface::ISensorDataListener
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

    virtual void onNewSensorData(const Interface::ISensorDriver* sensor,
        const std::vector<Interface::ISensorData*>& data, bool first) override;

    virtual void handleEvent(const G2SEvent::GuiRun& event) override;
    virtual void handleEvent(const G2SEvent::GuiStop& event) override;
    virtual void handleEvent(const G2SEvent::TrackStart& event) override;
    virtual void handleEvent(const G2SEvent::TrackStop& event) override;
    virtual void handleEvent(const G2SEvent::SettingsSave& event) override;
    virtual void handleEvent(const G2SEvent::InternalRefresh& event) override;


    // Force refresh service state
    void refreshService();

    void sendInitialInfoToGui();



    const IKernel&            mKernel;
    std::shared_ptr<GSModel>  mGSModel;
    bool                      mTerminate;
    bool                      mGUIStarted;

    Settings mSettings;
    SettingsSerializer mSettingsSerializer;
    
    ActivitySummary mSummary;
    ActivitySummarySerializer mActivitySummarySerializer;
    TrackMapBuilder mTrackMapBuilder;

    // Sensors
    Interface::ISensorDriver* mGpsSensor = nullptr;
    Interface::ISensorDriver* mStepCounterSensor = nullptr;
    Interface::ISensorDriver* mFloorCounterSensor = nullptr;
    Interface::ISensorDriver* mAltimeterSensor = nullptr;
    Interface::ISensorDriver* mHrSensor = nullptr;

    static constexpr uint32_t skInitialSamplePeriod = 1000;
    static constexpr uint32_t skSamplePeriod = 10000;
    static constexpr uint32_t skSampleLatency = 0;

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
        float    steps;     // steps
        uint32_t timestamp; // ms

        // Working data
        float    initialSteps = -1; // < 0 - not avaliable
    } mStepCounter {};

    struct {
        // Last sensor data
        float    floors;     // steps
        uint32_t timestamp; // ms

        // Working data
        float    initialFloors = -1; // < 0 - not avaliable
    } mFloorsCounter {};

    struct {
        // Last sensor data
        float    altitude;  // meters
        uint32_t timestamp; // ms

        // Working data
        float initialAltitude = -1; // < 0 - not avaliable
    } mAltimeter {};
    
    struct {
        // Last sensor data
        float    hr;        // bpm
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
    TrackMapBuilder::GpsPoint mPrevGpsPoint {};

    Track::Data mTrackData {};

    std::vector<ActivityWriter::LapData> mLaps;
    std::vector<ActivityWriter::PointData> mPoints;

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
