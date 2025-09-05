#include <gui/alarm_screen/AlarmView.hpp>
#include <gui/alarm_screen/AlarmPresenter.hpp>

AlarmPresenter::AlarmPresenter(AlarmView& v)
    : view(v)
{

}

void AlarmPresenter::activate()
{
    model->playAlarm();
}

void AlarmPresenter::deactivate()
{
    // If the alarm is not turned off by the user and the screen has
    // switched to a higher priority, then stop it
    if (model->getActiveAlarm().on) {
        model->stopAlarm();
    }
}

void AlarmPresenter::stop()
{
    model->stopAlarm();
    model->switchToNextPriorityScreen();
}

void AlarmPresenter::snooze()
{
    model->snoozeAlarm();
    model->switchToNextPriorityScreen();
}
