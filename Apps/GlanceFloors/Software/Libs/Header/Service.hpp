#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include <ctime>

#include "SDK/Kernel/Kernel.hpp"

#include "SDK/Glance/GlanceControl.hpp"

//#include "SDK/Interfaces/ISensorDriver.hpp"
//#include "SDK/Interfaces/ISensorDataListener.hpp"
//#include "SDK/SensorLayer/SensorDriverConnection.hpp"

class Service //: public SDK::Interface::ISensorDataListener
{
public:
    Service(SDK::Kernel &kernel);

    virtual ~Service();

    void run();

private:
    const SDK::Kernel&  mKernel;


    void connect();
    void disconnect();

    // ISensorDataListener implementation
//    virtual void onSdlNewData(const SDK::Interface::ISensorDriver*             sensor,
//                              const std::vector<SDK::Interface::ISensorData*>& data,
//                              bool                                             first) override;

    void onGlanceTick();
    bool configGui();
    void createGuiControls();

    const char*              mName;
    uint32_t                 mMaxControls;
    SDK::Glance::Form        mGlanceUI;
    SDK::Glance::ControlText mGlanceTitle;
    SDK::Glance::ControlText mGlanceValue;

    SDK::CommandHandler      mCH;

//    SDK::Sensor::DriverConnection mFloorsSensor;
    uint32_t mFloorsValue;

};

#endif
