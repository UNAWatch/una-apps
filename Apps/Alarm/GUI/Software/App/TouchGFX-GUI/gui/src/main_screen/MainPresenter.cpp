#include <gui/main_screen/MainView.hpp>
#include <gui/main_screen/MainPresenter.hpp>

MainPresenter::MainPresenter(MainView& v)
    : view(v)
{

}

void MainPresenter::activate()
{
    // Reset idle timer
    model->resetIdleTimer();

    view.updateAlarmList(model->getAlarmList());
    view.setActiveAlarm(model->alarmGetEditId());
}

void MainPresenter::deactivate()
{

}

void MainPresenter::onAlarmListUpdated(const std::vector<AppType::Alarm>& list) 
{
    view.updateAlarmList(list);
}

void MainPresenter::setAlarmIdForEdit(size_t id)
{
    model->alarmSetEditId(id);
}

void MainPresenter::exitApp()
{
    model->exitApp();
}