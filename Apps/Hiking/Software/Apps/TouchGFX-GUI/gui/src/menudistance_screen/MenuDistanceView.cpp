#include <gui/menudistance_screen/MenuDistanceView.hpp>

MenuDistanceView::MenuDistanceView()
{

}

void MenuDistanceView::setupScreen()
{
    MenuDistanceViewBase::setupScreen();

    menu.setTitle(T_TEXT_DISTANCE_UC);
    menu.setNumberOfItems(App::Menu::Settings::Alerts::Distance::ID_COUNT);

    update();
}

void MenuDistanceView::tearDownScreen()
{
    MenuDistanceViewBase::tearDownScreen();
}

void MenuDistanceView::setDistanceUnits(float km, bool isImperial)
{
    mUnitsImperial = isImperial;

    update();

    float distance = isImperial ? App::Utils::km2mi(km) : km;
    uint32_t distanceId = App::Menu::RoundToNearestIndex(App::Menu::kDistanceList,
        App::Menu::Settings::Alerts::Distance::ID_COUNT, distance);

    menu.selectItem(distanceId);
}

void MenuDistanceView::handleKeyEvent(uint8_t key)
{
    if (key == Gui::Config::Button::L1) {
        menu.selectPrev();
    }

    if (key == Gui::Config::Button::L2) {
        menu.selectNext();
    }

    if (key == Gui::Config::Button::R1) {
        uint16_t id = menu.getSelectedItem();
        if (mUnitsImperial) {
            presenter->saveDistance(App::Utils::mi2km(App::Menu::kDistanceList[id]));
        } else { 
            presenter->saveDistance(App::Menu::kDistanceList[id]);
        }
        application().gotoMenuDistanceSavedScreenNoTransition();
    }

    if (key == Gui::Config::Button::R2) {
        application().gotoMenuAlertsScreenNoTransition();
    }
}

void MenuDistanceView::update()
{
    MenuItemSelected *pS = nullptr;
    MenuItemNotSelected *pN = nullptr;

    const uint16_t kBuffSize = 32;
    touchgfx::Unicode::UnicodeChar buffer[kBuffSize] { };

    for (int i = 0; i < App::Menu::Settings::Alerts::Distance::ID_COUNT; i++) {
        pS = menu.getSelectedItem(i);
        pN = menu.getNotSelectedItem(i);

        if (i == 0) {
            Unicode::snprintf(buffer, kBuffSize, "%s", touchgfx::TypedText(T_TEXT_OFF_UC).getText());
        } else {
            if (mUnitsImperial) {
                touchgfx::TypedTextId txt = App::Menu::kDistanceList[i] > 1 ?
                    T_TEXT_MILES : T_TEXT_MILE;

                Unicode::snprintf(buffer, kBuffSize, "%d %s",
                    App::Menu::kDistanceList[i],
                    touchgfx::TypedText(txt).getText());

            } else {
                Unicode::snprintf(buffer, kBuffSize, "%d %s",
                    App::Menu::kDistanceList[i],
                    touchgfx::TypedText(T_TEXT_KM).getText());
            }
        }

        pS->config(buffer);
        pN->config(buffer);
    }

    menu.invalidate();
}