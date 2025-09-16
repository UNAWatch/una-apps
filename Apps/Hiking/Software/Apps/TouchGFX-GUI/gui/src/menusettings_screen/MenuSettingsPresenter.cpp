#include <gui/menusettings_screen/MenuSettingsView.hpp>
#include <gui/menusettings_screen/MenuSettingsPresenter.hpp>

MenuSettingsPresenter::MenuSettingsPresenter(MenuSettingsView& v)
    : view(v)
{

}

void MenuSettingsPresenter::activate()
{
    // Set current menu position
    view.setPositionId(model->getMenuPosMenuSettings());

    // Reset nested menu position
    model->setMenuPosMenuAlerts(0);

    // Reset idle timer
    model->resetIdleTimer();

    view.setGpsFix(model->getGpsFix());
    view.setAutoPause(model->getSettings().autoPauseEn);
    view.setPhoneNotif(model->getSettings().phoneNotifEn);
}

void MenuSettingsPresenter::deactivate()
{
    model->setMenuPosMenuSettings(view.getPositionId());
}

void MenuSettingsPresenter::onGpsFix(bool acquired)
{
    view.setGpsFix(acquired);
}

void MenuSettingsPresenter::saveAutoPause(bool state)
{
    Settings sett = model->getSettings();
    sett.autoPauseEn = state;
    model->setSettings(sett);
}

void MenuSettingsPresenter::savePhoneNotif(bool state)
{
    Settings sett = model->getSettings();
    sett.phoneNotifEn = state;
    model->setSettings(sett);
}
