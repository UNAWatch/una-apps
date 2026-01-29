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
    void saveJson();
    void checkDayRollover();

    struct StrainSample {
        uint64_t timestamp;
        uint16_t hr;
        float strain_delta;
    };

    const SDK::Kernel&       mKernel;
    const char*              mName;
    SDK::Glance::Form        mGlanceUI;
    SDK::Glance::ControlText mGlanceTitle;
    SDK::Glance::ControlText mGlanceValue;

    SDK::Sensor::Connection mSensorHR;
    SDK::Sensor::Connection mSensorActivity;

    // Accumulators
    float mTotalStrain = 0.0f;
    float mSumHR = 0.0f;
    uint16_t mMaxHR = 0;
    uint32_t mActiveMin = 0;
    uint32_t mSampleCount = 0;
    std::vector<StrainSample> mSamples;
    std::time_t mLastSaveTime = 0;
    uint32_t mTickCount = 0;
    char mJsonPath[64] = {};
    char mCurrentDate[11] = {};
};

#endif