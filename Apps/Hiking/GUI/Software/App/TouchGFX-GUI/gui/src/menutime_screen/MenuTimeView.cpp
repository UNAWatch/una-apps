#include <gui/menutime_screen/MenuTimeView.hpp>

MenuTimeView::MenuTimeView()
{

}

void MenuTimeView::setupScreen()
{
    MenuTimeViewBase::setupScreen();

    menu.setTitle(T_TEXT_TIME_UC);
    menu.setNumberOfItems(Gui::Menu::Settings::Alerts::Time::ID_COUNT);

    MenuItemSelected *pS = nullptr;
    MenuItemNotSelected *pN = nullptr;

    const uint16_t kBuffSize = 32;
    touchgfx::Unicode::UnicodeChar buffer[kBuffSize] { };

    for (int i = 0; i < Gui::Menu::Settings::Alerts::Time::ID_COUNT; i++) {
        pS = menu.getSelectedItem(i);

        if (i == 0) {
            Unicode::snprintf(buffer, kBuffSize, "%s", touchgfx::TypedText(T_TEXT_OFF_UC).getText());
        } else {
            touchgfx::TypedTextId txt = Gui::kTimeList[i] > 1 ? T_TEXT_MINUTES_LC : T_TEXT_MINUTE_LC;

            Unicode::snprintf(buffer, kBuffSize, "%d %s",
                Gui::kTimeList[i],
                touchgfx::TypedText(txt).getText());
        }

        pS->config(buffer);
    }

    for (int i = 0; i < Gui::Menu::Settings::Alerts::Time::ID_COUNT; i++) {
        pN = menu.getNotSelectedItem(i);

        if (i == 0) {
            Unicode::snprintf(buffer, kBuffSize, "%s", touchgfx::TypedText(T_TEXT_OFF_UC).getText());
        } else {
            Unicode::snprintf(buffer, kBuffSize, "%d %s",
                Gui::kTimeList[i],
                touchgfx::TypedText(T_TEXT_MIN_DOT).getText());
        }
        pN->config(buffer);
    }

    menu.invalidate();
}

void MenuTimeView::tearDownScreen()
{
    MenuTimeViewBase::tearDownScreen();
}

void MenuTimeView::setTime(uint32_t minutes)
{
    uint32_t timeId = Gui::Utils::RoundToNearestIndex(Gui::kTimeList,
        Gui::Menu::Settings::Alerts::Time::ID_COUNT, static_cast<float>(minutes));

    menu.selectItem(timeId);
}

void MenuTimeView::handleKeyEvent(uint8_t key)
{
    if (key == Gui::Config::Button::L1) {
        menu.selectPrev();
    }

    if (key == Gui::Config::Button::L2) {
        menu.selectNext();
    }

    if (key == Gui::Config::Button::R1) {
        uint16_t id = menu.getSelectedItem();
        presenter->saveTime(Gui::kTimeList[id]);
        application().gotoMenuTimeSavedScreenNoTransition();
    }

    if (key == Gui::Config::Button::R2) {
        application().gotoMenuAlertsScreenNoTransition();
    }
}