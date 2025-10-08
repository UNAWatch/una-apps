#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include <ctime>

#include "SDK/Kernel/KernelProviderService.hpp"

#include "SDK/Interfaces/IGlance.hpp"
#include "SDK/Glance/GlanceControl.hpp"

#include "SDK/Interfaces/ISensorDriver.hpp"
#include "SDK/Interfaces/ISensorDataListener.hpp"

class Service : public SDK::Interface::IApp::Callback,
                public SDK::Interface::IGlance
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

    void createUI();

    SDK::Glance::Form        mUI;
    SDK::Glance::ControlText mTextTime;
    SDK::Glance::ControlText mTextTemperature;
    float                    mTemperature;
    std::tm                  mTime;
};

#endif
