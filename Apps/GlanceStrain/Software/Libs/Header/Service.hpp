#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include <vector>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <inttypes.h>

#include "SDK/Kernel/Kernel.hpp"
#include "SDK/Glance/GlanceControl.hpp"
#include "SDK/SensorLayer/SensorConnection.hpp"
#include "SDK/Interfaces/ISensorDataListener.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRate.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserActivity.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserTouch.hpp"
#include "SDK/FitHelper/FitHelper.hpp"

class Service : public SDK::Interface::ISensorDataListener
{
public:
    Service(SDK::Kernel &kernel);

    virtual ~Service();

    void run();

private:
    void connect();
    void disconnect();

    // ISensorDataListener implementation
    void onSdlNewData(uint16_t                 handle,
                      const SDK::Sensor::Data* data,
                      uint16_t                 count,
                      uint16_t                 stride) override;

    void onGlanceTick();
    bool configGui();
    void createGuiControls();
    void saveFit(bool force, bool finalizeDay);
    void checkDayRollover();
    void appendPendingRecords(SDK::Interface::IFile* fp);
    void writeFitDefinitions(SDK::Interface::IFile* fp, std::time_t timestamp);
    void writeFitSessionSummary(SDK::Interface::IFile* fp, std::time_t timestamp);
    void startNewSession(SDK::Interface::IFile* fp, std::time_t timestamp);
    void loadSessionIndex(SDK::Interface::IFile* fp);

    struct FitRecord {
        std::time_t timestamp;
        uint8_t     hr;
        float       strain_delta;
        uint32_t    active_min;
    };

    const SDK::Kernel&       mKernel;
    const char*              mName;
    SDK::Glance::Form        mGlanceUI;
    SDK::Glance::ControlText mGlanceTitle;
    SDK::Glance::ControlText mGlanceValue;

    SDK::Sensor::Connection mSensorHR;
    SDK::Sensor::Connection mSensorActivity;
    SDK::Sensor::Connection mSensorTouch;

    bool mGlanceActive = false;
    bool mIsOnHand = true;

    // Accumulators
    float mTotalStrain = 0.0f;
    float mSumHR = 0.0f;
    uint16_t mMaxHR = 0;
    uint32_t mActiveMin = 0;
    uint32_t mSampleCount = 0;
    std::vector<FitRecord> mPendingRecords;
    std::time_t mLastSaveTime = 0;
    bool mSessionOpen = false;
    uint16_t mLastHr = 0;
    char mFitPath[64] = {};
    char mCurrentDate[11] = {};
    std::time_t mDayStart = 0;
    bool mFitFileInitialized = false;
    bool mSessionIndexInitialized = false;
    uint16_t mSessionIndex = 0;
    bool mStrainLoaded = false;

    SDK::Component::FitHelper mFitFileID;
    SDK::Component::FitHelper mFitDeveloper;
    SDK::Component::FitHelper mFitRecord;
    SDK::Component::FitHelper mFitEvent;
    SDK::Component::FitHelper mFitSession;
    SDK::Component::FitHelper mFitActivity;
    SDK::Component::FitHelper mFitStrainField;
    SDK::Component::FitHelper mFitActiveField;

    static constexpr uint8_t skFileMsgNum     = 1;
    static constexpr uint8_t skDevelopMsgNum  = 2;
    static constexpr uint8_t skRecordMsgNum   = 3;
    static constexpr uint8_t skEventMsgNum    = 4;
    static constexpr uint8_t skSessionMsgNum  = 5;
    static constexpr uint8_t skActivityMsgNum = 6;
    static constexpr uint8_t skStrainMsgNum   = 7;
    static constexpr uint8_t skActiveMsgNum   = 8;

    static constexpr uint32_t skSamplePeriodSec = 5;
    static constexpr uint32_t skSaveIntervalSec = 3600;
};

#endif
