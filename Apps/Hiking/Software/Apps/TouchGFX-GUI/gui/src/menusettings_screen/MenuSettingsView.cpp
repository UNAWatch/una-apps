#include <gui/menusettings_screen/MenuSettingsView.hpp>

MenuSettingsView::MenuSettingsView()
{

}

void MenuSettingsView::setupScreen()
{
    MenuSettingsViewBase::setupScreen();

    menu.setTitle(T_TEXT_SETTINGS_UC);

    menu.setNumberOfItems(App::Menu::Settings::ID_COUNT);

    MenuItemSelected *pS = nullptr;
    MenuItemNotSelected *pN = nullptr;

    // ALERTS
    pS = menu.getSelectedItem(App::Menu::Settings::ID_ALERTS);
    pS->config(T_TEXT_ALERTS);
    pN = menu.getNotSelectedItem(App::Menu::Settings::ID_ALERTS);
    pN->config(T_TEXT_ALERTS);

    // AUTO PAUSE
    pS = menu.getSelectedItem(App::Menu::Settings::ID_AUTO_PAUSE);
    pS->configToggle(T_TEXT_AUTO_BR_PAUSE);
    pS->setMsgTypedTextId(T_TMP_SEMIBOLD_25);

    pN = menu.getNotSelectedItem(App::Menu::Settings::ID_AUTO_PAUSE);
    pN->configTip(T_TEXT_AUTO_PAUSE, T_TEXT_ON_UC);

    // PHONE NOTIF
    pS = menu.getSelectedItem(App::Menu::Settings::ID_PHONE_NOTIF);
    pS->configToggle(T_TEXT_PHONE_BR_NOTIF_DOT);
    pS->setMsgTypedTextId(T_TMP_SEMIBOLD_25);

    pN = menu.getNotSelectedItem(App::Menu::Settings::ID_PHONE_NOTIF);
    pN->configTip(T_TEXT_PHONE_NOTIF_DOT, T_TEXT_OFF_UC);

    setGpsFix(mGpsFix);
    setAutoPause(mAutoPause);
    setPhoneNotif(mPhoneNotif);

    menu.invalidate();
}

void MenuSettingsView::tearDownScreen()
{
    MenuSettingsViewBase::tearDownScreen();
}


void MenuSettingsView::setGpsFix(bool state)
{
    mGpsFix = state;
    if (mGpsFix) {
        menu.setInfoMsg(T_TEXT_SIGNAL_ACQUIRED);
    } else {
        menu.setInfoMsg(T_TEXT_ACQUIRING_SIGNAL);
    }
}

void MenuSettingsView::setAutoPause(bool state)
{
    mAutoPause = state;

    MenuItemSelected *pS = nullptr;
    MenuItemNotSelected *pN = nullptr;
    pS = menu.getSelectedItem(App::Menu::Settings::ID_AUTO_PAUSE);
    pN = menu.getNotSelectedItem(App::Menu::Settings::ID_AUTO_PAUSE);

    pS->setToggle(mAutoPause);
    pN->configTip(T_TEXT_AUTO_PAUSE, state ? T_TEXT_ON_UC : T_TEXT_OFF_UC);
    pN->setTipColor(state ? Gui::Config::Color::MENU_NOT_SELECTED_TIP_ON : Gui::Config::Color::MENU_NOT_SELECTED_TIP_OFF);

    menu.invalidate();
}

void MenuSettingsView::setPhoneNotif(bool state)
{
    mPhoneNotif = state;

    MenuItemSelected *pS = nullptr;
    MenuItemNotSelected *pN = nullptr;
    pS = menu.getSelectedItem(App::Menu::Settings::ID_PHONE_NOTIF);
    pN = menu.getNotSelectedItem(App::Menu::Settings::ID_PHONE_NOTIF);

    pS->setToggle(mPhoneNotif);
    pN->configTip(T_TEXT_PHONE_NOTIF_DOT, state ? T_TEXT_ON_UC : T_TEXT_OFF_UC);
    pN->setTipColor(state ? Gui::Config::Color::MENU_NOT_SELECTED_TIP_ON : Gui::Config::Color::MENU_NOT_SELECTED_TIP_OFF);

    menu.invalidate();
}

void MenuSettingsView::setPositionId(uint16_t id)
{
    menu.selectItem(id);
}

uint16_t MenuSettingsView::getPositionId()
{
    return menu.getSelectedItem();
}

void MenuSettingsView::handleKeyEvent(uint8_t key)
{
    if (key == Gui::Config::Button::L1) {
        menu.selectPrev();
    }

    if (key == Gui::Config::Button::L2) {
        menu.selectNext();
    }

    if (key == Gui::Config::Button::R1) {
        uint32_t id = menu.getSelectedItem();

        switch (id) {
            case App::Menu::Settings::ID_ALERTS:
                application().gotoMenuAlertsScreenNoTransition();
                break;
            case App::Menu::Settings::ID_AUTO_PAUSE:
                mAutoPause = !mAutoPause;
                setAutoPause(mAutoPause);
                presenter->saveAutoPause(mAutoPause);
                break;
            case App::Menu::Settings::ID_PHONE_NOTIF:
                mPhoneNotif = !mPhoneNotif;
                setPhoneNotif(mPhoneNotif);
                presenter->savePhoneNotif(mPhoneNotif);
                break;
            default:
                break;
        };
    }

    if (key == Gui::Config::Button::R2) {
        application().gotoEnterMenuScreenNoTransition();
    }
}
