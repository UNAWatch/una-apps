#include <gui/menudistance_screen/MenuDistanceView.hpp>
#include <gui/menudistance_screen/MenuDistancePresenter.hpp>

MenuDistancePresenter::MenuDistancePresenter(MenuDistanceView& v)
    : view(v)
{

}

void MenuDistancePresenter::activate()
{
    // Set current menu position and units
    view.setDistanceUnits(model->getSettings().alertDistance, model->isUnitsImperial());

    // Reset idle timer
    model->resetIdleTimer();
}

void MenuDistancePresenter::deactivate()
{

}

void MenuDistancePresenter::saveDistance(float km)
{
    Settings sett = model->getSettings();
    sett.alertDistance = km;
    model->setSettings(sett);
}