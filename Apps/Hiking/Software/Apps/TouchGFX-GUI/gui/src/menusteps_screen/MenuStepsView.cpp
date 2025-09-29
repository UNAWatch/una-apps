#include <gui/menusteps_screen/MenuStepsView.hpp>

MenuStepsView::MenuStepsView()
{

}

void MenuStepsView::setupScreen()
{
    MenuStepsViewBase::setupScreen();

    menu.setTitle(T_TEXT_STEPS_UC);
    menu.setNumberOfItems(App::Menu::Settings::Alerts::Steps::ID_COUNT);

    MenuItemSelected *pS = nullptr;
    MenuItemNotSelected *pN = nullptr;

    const uint16_t kBuffSize = 32;
    touchgfx::Unicode::UnicodeChar buffer[kBuffSize] { };

    for (int i = 0; i < App::Menu::Settings::Alerts::Steps::ID_COUNT; i++) {
        pS = menu.getSelectedItem(i);

        if (i == 0) {
            Unicode::snprintf(buffer, kBuffSize, "%s", touchgfx::TypedText(T_TEXT_OFF_UC).getText());
        } else {
            Unicode::snprintf(buffer, kBuffSize, "%d", App::Menu::kStepsList[i]);
        }

        pS->config(buffer);
    }

    for (int i = 0; i < App::Menu::Settings::Alerts::Steps::ID_COUNT; i++) {
        pN = menu.getNotSelectedItem(i);

        if (i == 0) {
            Unicode::snprintf(buffer, kBuffSize, "%s", touchgfx::TypedText(T_TEXT_OFF_UC).getText());
        } else {
            Unicode::snprintf(buffer, kBuffSize, "%d", App::Menu::kStepsList[i]);
        }
        pN->config(buffer);
    }

    menu.invalidate();
}

void MenuStepsView::tearDownScreen()
{
    MenuStepsViewBase::tearDownScreen();
}

void MenuStepsView::setSteps(uint32_t steps)
{
    uint32_t stepsId = App::Menu::RoundToNearestIndex(App::Menu::kStepsList,
        App::Menu::Settings::Alerts::Steps::ID_COUNT, static_cast<float>(steps));

    menu.selectItem(stepsId);
}

void MenuStepsView::handleKeyEvent(uint8_t key)
{
    if (key == Gui::Config::Button::L1) {
        menu.selectPrev();
    }

    if (key == Gui::Config::Button::L2) {
        menu.selectNext();
    }

    if (key == Gui::Config::Button::R1) {
        uint16_t id = menu.getSelectedItem();
        presenter->saveSteps(App::Menu::kStepsList[id]);
        application().gotoMenuStepsSavedScreenNoTransition();
    }

    if (key == Gui::Config::Button::R2) {
        application().gotoMenuAlertsScreenNoTransition();
    }
}
