#include <gui/start_screen/StartView.hpp>
#include <gui/start_screen/StartPresenter.hpp>

StartPresenter::StartPresenter(StartView& v)
    : view(v)
{

}

void StartPresenter::activate()
{

}

void StartPresenter::deactivate()
{

}

void StartPresenter::onAlarmActivated(const AppType::Alarm& alarm) {
    model->application().gotoAlarmScreenNoTransition();
}

void StartPresenter::onRunForSettings() {
    model->application().gotoMainScreenNoTransition();
}