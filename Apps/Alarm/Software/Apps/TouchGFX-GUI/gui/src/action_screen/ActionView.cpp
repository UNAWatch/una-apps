#include <gui/action_screen/ActionView.hpp>

ActionView::ActionView()
{

}

void ActionView::setupScreen()
{
    ActionViewBase::setupScreen();

    title.set(T_TEXT_ALARM_UC);

    buttons.setL1(Buttons::WHITE);
    buttons.setL2(Buttons::WHITE);
    buttons.setR1(Buttons::AMBER);
    buttons.setR2(Buttons::WHITE);

    menu.setNumberOfItems(Gui::Config::Menu::Action::ACTION_COUNT);

    MenuWheelItemSelected* pS = nullptr;
    MenuWheelItemNotSelected* pN = nullptr;

    pS = menu.getSelectedItem(Gui::Config::Menu::Action::ACTION_ON_OF);
    pS->configToggle(T_TEXT_OFF_UC, T_TEXT_ON_UC);

    pN = menu.getNotSelectedItem(Gui::Config::Menu::Action::ACTION_ON_OF);
    pN->config(T_TEXT_OFF_ON);

    pS = menu.getSelectedItem(Gui::Config::Menu::Action::ACTION_EDIT);
    pS->config(T_TEXT_EDIT);

    pN = menu.getNotSelectedItem(Gui::Config::Menu::Action::ACTION_EDIT);
    pN->config(T_TEXT_EDIT);

    pS = menu.getSelectedItem(Gui::Config::Menu::Action::ACTION_DELETE);
    pS->config(T_TEXT_DELETE);

    pN = menu.getNotSelectedItem(Gui::Config::Menu::Action::ACTION_DELETE);
    pN->config(T_TEXT_DELETE);

    menu.invalidate();
}

void ActionView::tearDownScreen()
{
    ActionViewBase::tearDownScreen();
}

void ActionView::setAlarmState(size_t id, bool state)
{
    mId = id;
    Unicode::snprintf(valueAlarmBuffer, VALUEALARM_SIZE, "%d", mId + 1);
    valueAlarm.invalidate();

    MenuWheelItemSelected* pS = menu.getSelectedItem(Gui::Config::Menu::Action::ACTION_ON_OF);
    pS->setToggle(state);
    menu.invalidate();
}

void ActionView::handleKeyEvent(uint8_t key)
{
    if (key == Gui::Config::Button::L1) {
        menu.selectPrev();
    }

    if (key == Gui::Config::Button::L2) {
        menu.selectNext();
    }

    if (key == Gui::Config::Button::R1) {
        int32_t id = menu.getSelectedItem();

        switch (id) {
        case Gui::Config::Menu::Action::ACTION_ON_OF:
        {
            MenuWheelItemSelected* pS = menu.getSelectedItem(Gui::Config::Menu::Action::ACTION_ON_OF);
            bool isOn = !pS->getToggle();
            pS->setToggle(isOn);
            menu.invalidate();
            presenter->save(mId, isOn);
        }
        break;
        case Gui::Config::Menu::Action::ACTION_EDIT:
            application().gotoSetTimeScreenNoTransition();

            break;
        case Gui::Config::Menu::Action::ACTION_DELETE:
            application().gotoDeletedScreenNoTransition();
            break;

        default:
            break;
        };
    }

    if (key == Gui::Config::Button::R2) {
        application().gotoMainScreenNoTransition();
    }
}