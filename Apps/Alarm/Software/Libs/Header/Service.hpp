#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include "SDK/GSModel/GSModel.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"

#include "AlarmManager.hpp"

class Service : public IServiceModelHandler,
    public SDK::Interface::IApp::Callback,
                public AlarmManager::AlarmCallback
{
public:
    Service();

    virtual ~Service() = default;

    void run();

    // User-defined event handlers
    virtual void handleEvent(const G2SEvent::AlarmSaveList& event) override;
    virtual void handleEvent(const G2SEvent::AlarmActiveteEffect& event) override;
    virtual void handleEvent(const G2SEvent::AlarmStop& event) override;
    virtual void handleEvent(const G2SEvent::AlarmStopAll& event) override;
    virtual void handleEvent(const G2SEvent::AlarmSnooze& event) override;
    virtual void handleEvent(const G2SEvent::AlarmSnoozeAll& event) override;

private:
    const SDK::Kernel&  mKernel;
    GSModel             mGSModel;

    bool                mTerminate;
    bool                mGUIStarted;

    // IUserApp::Callback implementation
    virtual void onStart()    override;
    virtual void onStop()     override;
    virtual void onStartGUI() override;
    virtual void onStopGUI()  override;

    // AlarmManager::AlarmCallback implementation
    virtual void onAlarm(const AppType::Alarm& alarm) override;
    virtual void onListChanged(const std::vector<AppType::Alarm>& list) override;

    AlarmManager        mAlarmManager;
    AppType::Alarm      mActiveAlarm;
};

#endif
