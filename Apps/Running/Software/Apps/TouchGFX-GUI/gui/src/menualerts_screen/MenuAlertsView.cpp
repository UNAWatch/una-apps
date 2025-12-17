#include <gui/menualerts_screen/MenuAlertsView.hpp>

MenuAlertsView::MenuAlertsView()
{

}

void MenuAlertsView::setupScreen()
{
    MenuAlertsViewBase::setupScreen();

    menu.setTitle(T_TEXT_LAP_ALERTS_UC);

    menu.setNumberOfItems(App::Menu::Settings::Alerts::ID_COUNT);
    update();
}

void MenuAlertsView::tearDownScreen()
{
    MenuAlertsViewBase::tearDownScreen();
}

void MenuAlertsView::setUnitsImperial(bool isImperial)
{
    mUnitsImperial = isImperial;
    update();
}

void MenuAlertsView::setDistance(float km)
{
    mDistanceKm = km;
    update();
}

void MenuAlertsView::setTime(uint32_t minutes)
{
    mTime = minutes;
    update();
}

void MenuAlertsView::setPositionId(uint16_t id)
{
    menu.selectItem(id);
}

uint16_t MenuAlertsView::getPositionId()
{
    return menu.getSelectedItem();
}

void MenuAlertsView::handleKeyEvent(uint8_t key)
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
            case App::Menu::Settings::Alerts::ID_DISTANCE:
                application().gotoMenuDistanceScreenNoTransition();
                break;
            case App::Menu::Settings::Alerts::ID_TIME:
                application().gotoMenuTimeScreenNoTransition();
                break;
            default:
                break;
        };
    }

    if (key == Gui::Config::Button::R2) {
        application().gotoMenuSettingsScreenNoTransition();
    }
}

void MenuAlertsView::update()
{
    MenuItemSelected *pS = nullptr;
    MenuItemNotSelected *pN = nullptr;

    const uint16_t kBuffSize = 32;
    touchgfx::Unicode::UnicodeChar buffer[kBuffSize] {};

    float distance = mUnitsImperial ? App::Utils::km2mi(mDistanceKm) : mDistanceKm;
    uint32_t distanceId = App::Menu::RoundToNearestIndex(App::Menu::kDistanceList,
        App::Menu::Settings::Alerts::Distance::ID_COUNT, distance);

    uint32_t timeId = App::Menu::RoundToNearestIndex(App::Menu::kTimeList,
        App::Menu::Settings::Alerts::Time::ID_COUNT, static_cast<float>(mTime));


    // DISTANCE
    if (distanceId == App::Menu::Settings::Alerts::Distance::ID_OFF) {
        Unicode::snprintf(buffer, kBuffSize, "%s", touchgfx::TypedText(T_TEXT_OFF_UC).getText());
    } else {
        float diff = std::abs(static_cast<float>(App::Menu::kDistanceList[distanceId]) - distance);
 
        if (diff > 0.01f) {
            // Show actual value
            Unicode::snprintfFloat(buffer, kBuffSize, "%.1f", distance);
        } else { 
            // Show value from the list
            Unicode::snprintf(buffer, kBuffSize, "%d", App::Menu::kDistanceList[distanceId]);
        }
        uint32_t offset = Unicode::strlen(buffer);

        if (mUnitsImperial) {
            touchgfx::TypedTextId txt = App::Menu::kDistanceList[distanceId] > 1 ?
                T_TEXT_MILES : T_TEXT_MILE;

            Unicode::snprintf(&buffer[offset], kBuffSize - offset, " %s", touchgfx::TypedText(txt).getText());
        } else {
            Unicode::snprintf(&buffer[offset], kBuffSize - offset, " %s", touchgfx::TypedText(T_TEXT_KM).getText());
        }
    }

    pS = menu.getSelectedItem(App::Menu::Settings::Alerts::ID_DISTANCE);
    pS->configTip(T_TEXT_DISTANCE, buffer);

    pN = menu.getNotSelectedItem(App::Menu::Settings::Alerts::ID_DISTANCE);
    pN->configTip(T_TEXT_DISTANCE, buffer);
    pN->setTipColor(distanceId == App::Menu::Settings::Alerts::Distance::ID_OFF ?
        Gui::Config::Color::MENU_NOT_SELECTED_TIP_OFF : Gui::Config::Color::MENU_NOT_SELECTED_TIP_ON);


    // TIME
    if (timeId == App::Menu::Settings::Alerts::Time::ID_OFF) {
        Unicode::snprintf(buffer, kBuffSize, "%s", touchgfx::TypedText(T_TEXT_OFF_UC).getText());
    } else {
        Unicode::snprintf(buffer, kBuffSize, "%d %s", mTime, touchgfx::TypedText(T_TEXT_MIN_DOT).getText());
    }

    pS = menu.getSelectedItem(App::Menu::Settings::Alerts::ID_TIME);
    pS->configTip(T_TEXT_TIME, buffer);

    pN = menu.getNotSelectedItem(App::Menu::Settings::Alerts::ID_TIME);
    pN->configTip(T_TEXT_TIME, buffer);
    pN->setTipColor(timeId == App::Menu::Settings::Alerts::Time::ID_OFF ?
        Gui::Config::Color::MENU_NOT_SELECTED_TIP_OFF : Gui::Config::Color::MENU_NOT_SELECTED_TIP_ON);

    menu.invalidate();
}
