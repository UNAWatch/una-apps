#include <gui/menutime_screen/MenuTimeView.hpp>
#include <gui/menutime_screen/MenuTimePresenter.hpp>

MenuTimePresenter::MenuTimePresenter(MenuTimeView& v)
    : view(v)
{

}

void MenuTimePresenter::activate()
{
    // Set current menu position
    view.setTime(model->getSettings().alertTime);

    // Reset idle timer
    model->resetIdleTimer();
}

void MenuTimePresenter::deactivate()
{

}

void MenuTimePresenter::saveTime(uint32_t minutes)
{
    Settings sett = model->getSettings();
    sett.alertTime = minutes;
    model->setSettings(sett);
}