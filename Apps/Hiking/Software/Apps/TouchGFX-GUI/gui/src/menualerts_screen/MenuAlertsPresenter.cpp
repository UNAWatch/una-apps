#include <gui/menualerts_screen/MenuAlertsView.hpp>
#include <gui/menualerts_screen/MenuAlertsPresenter.hpp>

MenuAlertsPresenter::MenuAlertsPresenter(MenuAlertsView& v)
    : view(v)
{

}

void MenuAlertsPresenter::activate()
{
    // Set current menu position
    view.setPositionId(model->getMenuPosMenuAlerts());

    // Reset idle timer
    model->resetIdleTimer();

    view.setUnitsImperial(model->isUnitsImperial());
    view.setSteps(model->getSettings().alertSteps);
    view.setDistance(model->getSettings().alertDistance);
    view.setTime(model->getSettings().alertTime);
}

void MenuAlertsPresenter::deactivate()
{
    model->setMenuPosMenuAlerts(view.getPositionId());
}
