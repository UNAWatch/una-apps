#include "Service.hpp"

#include <stdio.h>

#define LOG_MODULE_PRX      LOG_TAG"Service"
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
        localtime_s(&tmNow, &t);    // TODO: adjust fo real function
#else
        localtime_r(&t, &tmNow);
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

    mKernel.backlight.on(3000);

    bool isVibro = event.alarm.effect == AppType::Alarm::Effect::EFFECT_BEEP_AND_VIBRO ||
            event.alarm.effect == AppType::Alarm::Effect::EFFECT_VIBRO;

    bool isBuzzer = event.alarm.effect == AppType::Alarm::Effect::EFFECT_BEEP_AND_VIBRO ||
            event.alarm.effect == AppType::Alarm::Effect::EFFECT_BEEP;

    if (isVibro) {
        sdk::api::Vibro::Note vm[5] {};
        vm[0].effect = sdk::api::Vibro::ALERT_750MS_100;
        vm[1].pause = 250;
        vm[2].effect = sdk::api::Vibro::ALERT_750MS_100;
        vm[3].pause = 250;
        vm[4].effect = sdk::api::Vibro::ALERT_750MS_100;

        mKernel.vibro.play(vm, 5);
    }

    if (isBuzzer) {
        sdk::api::Buzzer::Note bm[5] {};
        bm[0].level = 3;
        bm[0].time = 750;
        bm[1].level = 0;
        bm[1].time = 250;
        bm[2].level = 3;
        bm[2].time = 750;
        bm[3].level = 0;
        bm[3].time = 250;
        bm[4].level = 3;
        bm[4].time = 750;

        mKernel.buzzer.play(bm, 5);
    }
}

void Service::handleEvent(const G2SEvent::AlarmStop& event)
{
    LOG_INFO("AlarmStop\n");
    mAlarmManager.disableAlarm(event.alarm);

    mKernel.buzzer.stop();
    mKernel.vibro.stop();

    refreshService();
}

void Service::handleEvent(const G2SEvent::AlarmStopAll& event)
{
    LOG_INFO("AlarmStopAll\n");
    mAlarmManager.disableAllActiveAlarm();

    mKernel.buzzer.stop();
    mKernel.vibro.stop();

    refreshService();
}

void Service::handleEvent(const G2SEvent::AlarmSnooze& event)
{
    LOG_INFO("AlarmSnooze\n");
    mAlarmManager.snoozeAlarm(event.alarm);

    mKernel.buzzer.stop();
    mKernel.vibro.stop();

    refreshService();
}

void Service::handleEvent(const G2SEvent::AlarmSnoozeAll& event)
{
    LOG_INFO("AlarmSnoozeAll\n");
    mAlarmManager.snoozeAllActiveAlarm();   

    mKernel.buzzer.stop();
    mKernel.vibro.stop();

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
