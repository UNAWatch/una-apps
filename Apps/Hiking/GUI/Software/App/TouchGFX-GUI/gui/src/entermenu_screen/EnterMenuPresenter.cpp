#include <gui/entermenu_screen/EnterMenuView.hpp>
#include <gui/entermenu_screen/EnterMenuPresenter.hpp>

EnterMenuPresenter::EnterMenuPresenter(EnterMenuView& v)
    : view(v)
{

}

void EnterMenuPresenter::activate()
{
    // Set current menu position
    view.setPositionId(model->getMenuPosEnterMenu());

    // Reset nested menu position
    model->setMenuPosMenuSettings(0);
    model->setMenuPosTrack(0);

    // Reset idle timer
    model->resetIdleTimer();

    view.setGpsFix(model->getGpsFix());
    view.setSummaryAvailable(model->trackIsSummaryAvailable());
}

void EnterMenuPresenter::deactivate()
{
    model->setMenuPosEnterMenu(view.getPositionId());
}

void EnterMenuPresenter::onGpsFix(bool acquired)
{
    view.setGpsFix(acquired);
}

void EnterMenuPresenter::startTrack()
{
    model->trackStart();
}

void EnterMenuPresenter::exitApp()
{
    model->exitApp();
}
