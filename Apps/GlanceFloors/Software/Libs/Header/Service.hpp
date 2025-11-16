#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include <ctime>

#include "SDK/Kernel/KernelProviderService.hpp"

#include "SDK/Interfaces/IGlance.hpp"
#include "SDK/Glance/GlanceControl.hpp"

#include "SDK/Interfaces/ISensorDriver.hpp"
#include "SDK/Interfaces/ISensorDataListener.hpp"
#include "SDK/SensorLayer/SensorDriverConnection.hpp"

class Service : public SDK::Interface::IApp::Callback,
                public SDK::Interface::IGlance,
                public SDK::Interface::ISensorDataListener
{
public:
    Service();

    ~Service() override = default;

    void run();

private:
    const SDK::Kernel&  mKernel;
    bool                mTerminate;

    // IApp::Callback implementation
    virtual void onCreate()  override;
    virtual void onStart()   override;
    virtual void onResume()  override;
    virtual void onStop()    override;
    virtual void onPause()   override;
    virtual void onDestroy() override;

    // IGlance implementation
    virtual IGlance::Info glanceGetInfo() override;
    virtual void glanceUpdate()           override;
    virtual void glanceClose()            override;

    // ISensorDataListener implementation
    virtual void onSdlNewData(const SDK::Interface::ISensorDriver*             sensor,
                              const std::vector<SDK::Interface::ISensorData*>& data,
                              bool                                             first) override;

    void createGlanceGUI();

    SDK::Glance::Form        mGlanceUI;
    SDK::Glance::ControlText mGlanceTitle;
    SDK::Glance::ControlText mGlanceValue;


    SDK::Sensor::DriverConnection mFloorsSensor;
    uint32_t mFloorsValue;

};

#endif
