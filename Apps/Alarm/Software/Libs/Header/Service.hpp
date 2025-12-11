
#pragma once

#include "SDK/Kernel/Kernel.hpp"

#include "AlarmManager.hpp"
#include "Commands.hpp"

class Service : public AlarmManager::AlarmCallback
{
public:
    Service(SDK::Kernel &kernel);

    virtual ~Service();

    void run();

    // User-defined event handlers
    void handleEvent(const CustomMessage::AlarmList& event);
    void handleEvent(const CustomMessage::AlarmActiveteEffect& event);
    void handleEvent(const CustomMessage::AlarmStop& event);
    void handleEvent(const CustomMessage::AlarmStopAll& event);
    void handleEvent(const CustomMessage::AlarmSnooze& event);
    void handleEvent(const CustomMessage::AlarmSnoozeAll& event);

private:
    const SDK::Kernel&  mKernel;
    bool                mGUIStarted;

    void onStartGUI();
    void onStopGUI();

    void stopAlarm();

    // AlarmManager::AlarmCallback implementation
    void onAlarm(const AppType::Alarm& alarm);
    void onListChanged(const std::vector<AppType::Alarm>& list);

    AlarmManager            mAlarmManager;
    AppType::Alarm          mActiveAlarm;
    CustomMessage::Sender   mGuiSender;
};
