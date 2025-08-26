#include <gui/settime_screen/SetTimeView.hpp>
#include <gui/settime_screen/SetTimePresenter.hpp>

SetTimePresenter::SetTimePresenter(SetTimeView& v)
    : view(v)
{

}

void SetTimePresenter::activate()
{
    // Reset idle timer
    model->resetIdleTimer();
    size_t id = model->alarmGetEditId();
    if (id < model->getAlarmList().size()) {
        const AppType::Alarm& alarm = model->getAlarmList()[id];
        view.set(alarm.timeHours, alarm.timeMinutes, alarm.repeat, alarm.effect);
    } else {
        // Start with default values for new alarm
        view.set(0, 0, AppType::Alarm::Repeat::REPEAT_NO, AppType::Alarm::Effect::EFFECT_BEEP_AND_VIBRO);
    }
}

void SetTimePresenter::deactivate()
{

}

void SetTimePresenter::exitScreen()
{
    model->switchToNextPriorityScreen();
}

void SetTimePresenter::save(uint8_t h, uint8_t m, AppType::Alarm::Repeat repeat, AppType::Alarm::Effect effect)
{
    AppType::Alarm alarm = {};

    alarm.timeHours = h;
    alarm.timeMinutes = m;
    alarm.repeat = repeat;
    alarm.effect = effect;
    alarm.on = true;
    model->saveAlarm(model->alarmGetEditId(), alarm);

    model->application().gotoSavedScreenNoTransition();
}