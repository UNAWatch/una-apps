#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include <ctime>

#include "SDK/Kernel/Kernel.hpp"
#include "SDK/Glance/GlanceControl.hpp"
#include "SDK/SensorLayer/SensorConnection.hpp"
#include "SDK/Interfaces/ISensorDataListener.hpp"

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

    void glanceUpdate();

    void onGlanceTick();
    bool configGui();
    void createGuiControls();

    const SDK::Kernel&       mKernel;
    const char*              mName;
    uint32_t                 mMaxControls;
    SDK::Glance::Form        mGlanceUI;
    SDK::Glance::ControlText mGlanceTitle;
    SDK::Glance::ControlText mGlanceValueAHR;
    SDK::Glance::ControlText mGlanceValueRHR;

    SDK::Sensor::Connection mSensorHRMetrics;
    float                   mAHRValue;
    float                   mRHRValue;
    bool                    mIsValid;
};

#endif
