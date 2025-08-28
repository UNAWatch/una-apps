#include <gui/menudistancesaved_screen/MenuDistanceSavedView.hpp>

MenuDistanceSavedView::MenuDistanceSavedView()
{

}

void MenuDistanceSavedView::setupScreen()
{
    MenuDistanceSavedViewBase::setupScreen();

    title.set(T_TEXT_DISTANCE_UC);
}

void MenuDistanceSavedView::tearDownScreen()
{
    MenuDistanceSavedViewBase::tearDownScreen();
}

void MenuDistanceSavedView::setDistanceUnits(float km, bool isImperial)
{
    float distance = isImperial ? Gui::Utils::km2mi(km) : km;

    uint32_t distanceId = Gui::Utils::RoundToNearestIndex(Gui::kDistanceList,
        Gui::Menu::Settings::Alerts::Distance::ID_COUNT, distance);

    if (distanceId == Gui::Menu::Settings::Alerts::Distance::ID_OFF) {
        Unicode::snprintf(msgBuffer, MSG_SIZE, "%s", touchgfx::TypedText(T_TEXT_OFF_UC).getText());
    } else {
        if (isImperial) {
            touchgfx::TypedTextId txt = Gui::kDistanceList[distanceId] > 1 ?
                T_TEXT_MILES : T_TEXT_MILE;

            Unicode::snprintf(msgBuffer, MSG_SIZE, "%d %s",
                Gui::kDistanceList[distanceId],
                touchgfx::TypedText(txt).getText());

        } else {
            Unicode::snprintf(msgBuffer, MSG_SIZE, "%d %s",
                Gui::kDistanceList[distanceId],
                touchgfx::TypedText(T_TEXT_KM).getText());
        }
    }
    msg.invalidate();
}

void MenuDistanceSavedView::handleTickEvent()
{
    if (mCounter > 0) {
        mCounter--;
    }
    if (mCounter == 0) {
        application().gotoMenuAlertsScreenNoTransition();
    }
}