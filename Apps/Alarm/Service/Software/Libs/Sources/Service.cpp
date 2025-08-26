#include "Service.hpp"

#include <stdio.h>

#define TAG                 "App::Alarm[S]"
#define LOG_MODULE_PRX      TAG"::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "Logger.h"

Service::Service(const IKernel& kernel)
        : mKernel(kernel)
        , mGSModel(std::make_shared<GSModel>(kernel, *this))
        , mTerminate(false)
        , mGUIStarted(false)
        , mAlarmManager(kernel)
        , mActiveAlarm()
{
    mKernel.app.registerApp(this);
    mKernel.sctrl.setContext(mGSModel);
}

void Service::run()
{
    mKernel.app.initialized();
    mAlarmManager.attachCallback(this);
    mAlarmManager.load();

    uint32_t startTime = mKernel.app.getTimeMs();

    while (!mTerminate) {
        std::tm tmNow{};
        time_t t = time(nullptr);
#if SIMULATOR
        time_t t = time(nullptr);
        localtime_s(&tmNow, &t);    // TODO: adjust fo real function
#else

        //localtime(&tmNow, &t);    // TODO: adjust fo real function
#endif


        uint32_t sleepTime = mAlarmManager.execute(tmNow);
        mGSModel->checkG2SEvents(sleepTime);

        if (mGUIStarted) {

        } else {
            // Just wait some time to see if GUI starts
            if (mKernel.app.getTimeMs() - startTime > 5000) {
                if (!mAlarmManager.hasActiveAlarms()) {
                    LOG_DEBUG("No active alarms and GUI not started, exiting service\n");
                    exit(0);
                }
            }
        }
    }

    // Cleanup
    mAlarmManager.attachCallback(nullptr);
}

void Service::handleEvent(const G2SEvent::Run& event)
{
    (void) event;

    mGUIStarted = true;

    // If there is an active alarm, send it to GUI first
    if (mActiveAlarm.on) {
        mGSModel->sendToGUI(S2GEvent::Alarm{ mActiveAlarm });
        mActiveAlarm = {}; // clear active alarm
    }

    // Send current alarm list to GUI
    mGSModel->sendToGUI(S2GEvent::AlarmList{ mAlarmManager.getAlarmList() });

    refreshService();
}

void Service::handleEvent(const G2SEvent::Stop& event)
{
    (void) event;

    mGUIStarted = false;

    refreshService();
}

// User-defined event handlers
void Service::handleEvent(const G2SEvent::AlarmSaveList& event)
{
    LOG_INFO("AlarmSaveList\n");
    mAlarmManager.saveAlarmList(event.list);

    refreshService();
}

void Service::handleEvent(const G2SEvent::AlarmActiveteEffect& event)
{
    LOG_INFO("AlarmActiveteEffect\n");
}

void Service::handleEvent(const G2SEvent::AlarmStop& event)
{
    LOG_INFO("AlarmStop\n");
    mAlarmManager.disableAlarm(event.alarm);

    refreshService();
}

void Service::handleEvent(const G2SEvent::AlarmStopAll& event)
{
    LOG_INFO("AlarmStopAll\n");
    mAlarmManager.disableAllActiveAlarm();

    refreshService();
}

void Service::handleEvent(const G2SEvent::AlarmSnooze& event)
{
    LOG_INFO("AlarmSnooze\n");
    mAlarmManager.snoozeAlarm(event.alarm);

    refreshService();
}

void Service::handleEvent(const G2SEvent::AlarmSnoozeAll& event)
{
    LOG_INFO("AlarmSnoozeAll\n");
    mAlarmManager.snoozeAllActiveAlarm();   

    refreshService();
}

void Service::handleEvent(const G2SEvent::InternalRefresh& event)
{
    // do nothing
}





void Service::onCreate()
{
    LOG_INFO("called\n");
}

void Service::onStart()
{
    LOG_INFO("called\n");
}

void Service::onResume()
{
    LOG_INFO("called\n");
}

void Service::onStop()
{
    LOG_INFO("called\n");
    mTerminate = true;
}

void Service::onPause()
{
    LOG_INFO("called\n");
}

void Service::onDestroy()
{
    LOG_INFO("called\n");
}

void Service::onAlarm(const AppType::Alarm& alarm)
{
    mKernel.sctrl.runGUI(mGSModel); // Make sure GUI is started
    
    if (mGUIStarted) {
        mActiveAlarm = {}; // clear active alarm
        mGSModel->sendToGUI(S2GEvent::Alarm{ alarm });
    } else {
        mActiveAlarm = alarm; // save active alarm
    }

}

void Service::onListChanged(const std::vector<AppType::Alarm>& list)
{
    if (mGUIStarted) {
        mGSModel->sendToGUI(S2GEvent::AlarmList{ list });
    }

    refreshService();
}


void Service::refreshService()
{
    mGSModel->sendToService(G2SEvent::InternalRefresh{});
}
