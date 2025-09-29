#include <gui/menustepssaved_screen/MenuStepsSavedView.hpp>

MenuStepsSavedView::MenuStepsSavedView()
{

}

void MenuStepsSavedView::setupScreen()
{
    MenuStepsSavedViewBase::setupScreen();

    title.set(T_TEXT_STEPS_UC);
}

void MenuStepsSavedView::tearDownScreen()
{
    MenuStepsSavedViewBase::tearDownScreen();
}

void MenuStepsSavedView::setSteps(uint32_t steps)
{
    uint32_t stepsId = App::Menu::RoundToNearestIndex(App::Menu::kStepsList,
        App::Menu::Settings::Alerts::Steps::ID_COUNT, static_cast<float>(steps));

    if (stepsId == App::Menu::Settings::Alerts::Steps::ID_OFF) {
        Unicode::snprintf(msgBuffer, MSG_SIZE, "%s", touchgfx::TypedText(T_TEXT_OFF_UC).getText());
    } else {
        Unicode::snprintf(msgBuffer, MSG_SIZE, "%d", steps);
    }
    msg.invalidate();
}

void MenuStepsSavedView::handleTickEvent()
{
    if (mCounter > 0) {
        mCounter--;
    }
    if (mCounter == 0) {
        application().gotoMenuAlertsScreenNoTransition();
    }
}