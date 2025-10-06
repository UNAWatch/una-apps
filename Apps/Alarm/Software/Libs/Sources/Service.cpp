#include "SDK/Interfaces/IBuzzer.hpp"
#include "SDK/Interfaces/IVibro.hpp"

#include "Service.hpp"
#include "GSModelEvents/G2SEvents.hpp"

#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(a[0]))

#define LOG_MODULE_PRX      "Service::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include <stdio.h>

Service::Service()
        : mKernel(SDK::KernelProviderService::GetInstance().getKernel())
        , mGSModel(*this)
        , mTerminate(false)
        , mGUIStarted(false)
        , mAlarmManager(mKernel)
        , mActiveAlarm()
{}

void Service::run()
{
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
        mGSModel.process(sleepTime);

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

void Service::onStartGUI()
{
    LOG_INFO("GUI started\n");
    mGUIStarted = true;

    // If there is an active alarm, send it to GUI first
    if (mActiveAlarm.on) {
        mGSModel.post(S2GEvent::Alarm{ mActiveAlarm });
        mActiveAlarm = {}; // clear active alarm
    }

    // Send current alarm list to GUI
    mGSModel.post(S2GEvent::AlarmList{ mAlarmManager.getAlarmList() });

    mGSModel.refresh();
}

void Service::onStopGUI()
{
    LOG_INFO("GUI stopped\n");
    mGUIStarted = false;
    mGSModel.refresh();
}

void Service::onStop()
{
    LOG_INFO("called\n");
    mTerminate = true;
}


// User-defined event handlers
void Service::handleEvent(const G2SEvent::AlarmSaveList& event)
{
    LOG_INFO("AlarmSaveList\n");
    mAlarmManager.saveAlarmList(event.list);

    mGSModel.refresh();
}

void Service::handleEvent(const G2SEvent::AlarmActiveteEffect& event)
{
    LOG_INFO("AlarmActiveteEffect\n");

    mKernel.backlight.on(3000);

    bool isVibro = event.alarm.effect == AppType::Alarm::Effect::EFFECT_BEEP_AND_VIBRO ||
            event.alarm.effect == AppType::Alarm::Effect::EFFECT_VIBRO;

    bool isBuzzer = event.alarm.effect == AppType::Alarm::Effect::EFFECT_BEEP_AND_VIBRO ||
            event.alarm.effect == AppType::Alarm::Effect::EFFECT_BEEP;

    LOG_INFO("isVibro isBuzzer : %d %d\n", (int)isVibro, (int)isBuzzer);

    if (isVibro) {
        SDK::Interface::IVibro::Note vm[5]{};
        vm[0].effect = SDK::Interface::IVibro::Effect::ALERT_750MS_100;
        vm[1].pause  = 250;
        vm[2].effect = SDK::Interface::IVibro::Effect::ALERT_750MS_100;
        vm[3].pause  = 250;
        vm[4].effect = SDK::Interface::IVibro::Effect::ALERT_750MS_100;

        mKernel.vibro.play(vm, ARRAY_SIZE(vm));
    }

    if (isBuzzer) {
        SDK::Interface::IBuzzer::Note bm[5]{};
        bm[0].level = 3;
        bm[0].time  = 750;
        bm[1].level = 0;
        bm[1].time  = 250;
        bm[2].level = 3;
        bm[2].time  = 750;
        bm[3].level = 0;
        bm[3].time  = 250;
        bm[4].level = 3;
        bm[4].time  = 750;

        mKernel.buzzer.play(bm, ARRAY_SIZE(bm));
    }
}

void Service::handleEvent(const G2SEvent::AlarmStop& event)
{
    LOG_INFO("AlarmStop\n");
    mAlarmManager.disableAlarm(event.alarm);

    mKernel.buzzer.stop();
    mKernel.vibro.stop();

    mGSModel.refresh();
}

void Service::handleEvent(const G2SEvent::AlarmStopAll& event)
{
    LOG_INFO("AlarmStopAll\n");
    mAlarmManager.disableAllActiveAlarm();

    mKernel.buzzer.stop();
    mKernel.vibro.stop();

    mGSModel.refresh();
}

void Service::handleEvent(const G2SEvent::AlarmSnooze& event)
{
    LOG_INFO("AlarmSnooze\n");
    mAlarmManager.snoozeAlarm(event.alarm);

    mKernel.buzzer.stop();
    mKernel.vibro.stop();

    mGSModel.refresh();
}

void Service::handleEvent(const G2SEvent::AlarmSnoozeAll& event)
{
    LOG_INFO("AlarmSnoozeAll\n");
    mAlarmManager.snoozeAllActiveAlarm();   

    mKernel.buzzer.stop();
    mKernel.vibro.stop();

    mGSModel.refresh();
}





void Service::onAlarm(const AppType::Alarm& alarm)
{
    //mKernel.sctrl.runGUI(mGSModel); // Make sure GUI is started
    
    if (mGUIStarted) {
        mActiveAlarm = {}; // clear active alarm
        mGSModel.post(S2GEvent::Alarm{ alarm });
    } else {
        mActiveAlarm = alarm; // save active alarm
    }

}

void Service::onListChanged(const std::vector<AppType::Alarm>& list)
{
    if (mGUIStarted) {
        mGSModel.post(S2GEvent::AlarmList{ list });
    }

    mGSModel.refresh();
}