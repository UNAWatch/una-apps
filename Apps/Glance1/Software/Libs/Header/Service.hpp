#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include "SDK/AppSystem/AppKernel.hpp"

#include "SDK/Interfaces/IGlance.hpp"
#include "SDK/Glance/GlanceControl.hpp"

#include "SDK/GSModel/GSModelHelper.hpp"
#include "SDK/Interfaces/ISensorDriver.hpp"
#include "SDK/Interfaces/ISensorDataListener.hpp"
#include "SDK/SwTimer/SwTimer.hpp"

class Service : public SDK::Interface::IUserApp::Callback,
                public SDK::Interface::IGlance
{
public:
    Service();

    ~Service() override = default;

    void run();

    GlanceControl_t* glanceGetControls(uint8_t& count) override;
    void             glanceUpdate()                    override;
    void             glanceClose()                     override;

private:
    void onCreate()  override;
    void onStart()   override;
    void onResume()  override;
    void onStop()    override;
    void onPause()   override;
    void onDestroy() override;

    void createUI();

    SDK::Kernel              mKernel;
    bool                     mTerminate;
    SDK::Glance::Form        mUI;
    SDK::Glance::ControlText mTextTime;
    SDK::Glance::ControlText mTextTemperature;
    float                    mTemperature;
    SDK::SwTimer             mUpdateTimer;
};

#endif
