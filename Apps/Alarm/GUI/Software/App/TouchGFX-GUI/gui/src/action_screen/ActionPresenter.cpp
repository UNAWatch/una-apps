#include <gui/action_screen/ActionView.hpp>
#include <gui/action_screen/ActionPresenter.hpp>

ActionPresenter::ActionPresenter(ActionView& v)
    : view(v)
{

}

void ActionPresenter::activate()
{
    // Reset idle timer
    model->resetIdleTimer();

    size_t id = model->alarmGetEditId();

    if (id < model->getAlarmList().size()) {
        view.setAlarmState(id, model->getAlarmList()[id].on);
    }
}

void ActionPresenter::deactivate()
{

}

void ActionPresenter::onAlarmListUpdated(const std::vector<AppType::Alarm>& list)
{
    size_t id = model->alarmGetEditId();

    if (id < model->getAlarmList().size()) {
        view.setAlarmState(id, model->getAlarmList()[id].on);
    }
}

void ActionPresenter::save(size_t id, bool state)
{
    if (id < model->getAlarmList().size()) {
        AppType::Alarm alarm = model->getAlarmList()[id];
        alarm.on = state;
        model->saveAlarm(id, alarm);
    }
}
