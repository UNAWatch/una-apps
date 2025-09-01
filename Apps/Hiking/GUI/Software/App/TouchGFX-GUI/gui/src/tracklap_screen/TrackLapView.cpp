#include <gui/tracklap_screen/TrackLapView.hpp>

TrackLapView::TrackLapView()
{

}

void TrackLapView::setupScreen()
{
    TrackLapViewBase::setupScreen();

    buttons.setL1(Buttons::NONE);
    buttons.setL2(Buttons::AMBER);
    buttons.setR1(Buttons::NONE);
    buttons.setR2(Buttons::NONE);
}

void TrackLapView::tearDownScreen()
{
    TrackLapViewBase::tearDownScreen();
}

void TrackLapView::setUnitsImperial(bool isImperial)
{
    mUnitsImperial = isImperial;

    if (mUnitsImperial) {
        Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_MI_DOT).getText());
    } else {
        Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_KM).getText());
    }
    distanceUnits.invalidate();
}

void TrackLapView::setLapNum(uint8_t n)
{
    const uint16_t kBuffSize = 10;
    touchgfx::Unicode::UnicodeChar buffer[kBuffSize] { };

    Unicode::snprintf(buffer, kBuffSize, "%s %d",
        touchgfx::TypedText(T_TEXT_LAP).getText(), n + 1);

    title.set(buffer);
}

void TrackLapView::setSteps(uint32_t steps)
{
    Unicode::snprintf(stepsValueBuffer, STEPSVALUE_SIZE, "%u", steps);
    stepsValue.invalidate();
}

void TrackLapView::setDistance(float km)
{
    if (mUnitsImperial) {
        Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", Gui::Utils::km2mi(km));
    } else {
        Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", km);
    }

    distanceValue.invalidate();
}

void TrackLapView::setTimer(uint32_t sec)
{
    Unicode::snprintf(timeValueBuffer, TIMEVALUE_SIZE, "%d:%02d",
        Gui::Utils::sec2hmsH(sec), Gui::Utils::sec2hmsM(sec));
    timeValue.invalidate();
}

void TrackLapView::handleTickEvent()
{
    if (mCounter > 0) {
        mCounter--;
    }
    if (mCounter == 0) {
        application().gotoTrackScreenNoTransition();
    }
}

void TrackLapView::handleKeyEvent(uint8_t key)
{
    if (key == Gui::Config::Button::L1) {

    }

    if (key == Gui::Config::Button::L2) {
        application().gotoTrackScreenNoTransition();
    }

    if (key == Gui::Config::Button::R1) {

    }

    if (key == Gui::Config::Button::R2) {

    }
}
