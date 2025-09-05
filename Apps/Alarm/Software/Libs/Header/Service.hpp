#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include "SDK/Interfaces/IKernel.hpp"
#include "GSModel.hpp"
#include "AlarmManager.hpp"

class Service : public IServiceModelHandler,
                public SDK::Interface::IUserApp::Callback,
                public AlarmManager::AlarmCallback
{
public:
    Service(const IKernel& kernel);

    virtual ~Service() = default;

    void run();

    virtual void handleEvent(const G2SEvent::Run& event);
    virtual void handleEvent(const G2SEvent::Stop& event);

    // User-defined event handlers
    virtual void handleEvent(const G2SEvent::AlarmSaveList& event) override;
    virtual void handleEvent(const G2SEvent::AlarmActiveteEffect& event) override;
    virtual void handleEvent(const G2SEvent::AlarmStop& event) override;
    virtual void handleEvent(const G2SEvent::AlarmStopAll& event) override;
    virtual void handleEvent(const G2SEvent::AlarmSnooze& event) override;
    virtual void handleEvent(const G2SEvent::AlarmSnoozeAll& event) override;
    virtual void handleEvent(const G2SEvent::InternalRefresh& event) override;

private:
    // IUserApp::Callback implementation
    void onCreate()  override;
    void onStart()   override;
    void onResume()  override;
    void onStop()    override;
    void onPause()   override;
    void onDestroy() override;

    // AlarmManager::AlarmCallback implementation
    virtual void onAlarm(const AppType::Alarm& alarm) override;
    virtual void onListChanged(const std::vector<AppType::Alarm>& list) override;

    
    // Force refresh service state
    void refreshService();


    const IKernel&            mKernel;
    std::shared_ptr<GSModel>  mGSModel;
    bool                      mTerminate;
    bool                      mGUIStarted;

    AlarmManager              mAlarmManager;
    AppType::Alarm            mActiveAlarm;
};

#endif
