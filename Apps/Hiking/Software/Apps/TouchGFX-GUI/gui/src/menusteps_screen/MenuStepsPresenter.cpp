#include <gui/menusteps_screen/MenuStepsView.hpp>
#include <gui/menusteps_screen/MenuStepsPresenter.hpp>

MenuStepsPresenter::MenuStepsPresenter(MenuStepsView& v)
    : view(v)
{

}

void MenuStepsPresenter::activate()
{
    // Set current menu position
    view.setSteps(model->getSettings().alertSteps);

    // Reset idle timer
    model->resetIdleTimer();
}

void MenuStepsPresenter::deactivate()
{

}

void MenuStepsPresenter::saveSteps(uint32_t steps)
{
    Settings sett = model->getSettings();
    sett.alertSteps = steps;
    model->setSettings(sett);
}