#include <gui/menutimesaved_screen/MenuTimeSavedView.hpp>

MenuTimeSavedView::MenuTimeSavedView()
{

}

void MenuTimeSavedView::setupScreen()
{
    MenuTimeSavedViewBase::setupScreen();

    title.set(T_TEXT_TIME_UC);
}

void MenuTimeSavedView::tearDownScreen()
{
    MenuTimeSavedViewBase::tearDownScreen();
}

void MenuTimeSavedView::setTime(uint32_t minutes)
{
    uint32_t timeId = App::Menu::RoundToNearestIndex(App::Menu::kTimeList,
        App::Menu::Settings::Alerts::Time::ID_COUNT, static_cast<float>(minutes));

    if (timeId == App::Menu::Settings::Alerts::Time::ID_OFF) {
        Unicode::snprintf(msgBuffer, MSG_SIZE, "%s", touchgfx::TypedText(T_TEXT_OFF_UC).getText());
    } else {
        Unicode::snprintf(msgBuffer, MSG_SIZE, "%d %s", minutes, touchgfx::TypedText(T_TEXT_MIN_DOT).getText());
    }
    msg.invalidate();
}

void MenuTimeSavedView::handleTickEvent()
{
    if (mCounter > 0) {
        mCounter--;
    }
    if (mCounter == 0) {
        application().gotoMenuAlertsScreenNoTransition();
    }
}