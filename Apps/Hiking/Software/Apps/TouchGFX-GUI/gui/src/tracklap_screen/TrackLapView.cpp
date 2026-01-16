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
}

void TrackLapView::setLapNum(uint32_t n)
{
    const uint16_t kBuffSize = 16;
    touchgfx::Unicode::UnicodeChar buffer[kBuffSize] { };

    Unicode::snprintf(buffer, kBuffSize, "%s %u", touchgfx::TypedText(T_TEXT_LAP).getText(), n + 1);

    title.set(buffer);
}

void TrackLapView::setSteps(uint32_t steps)
{
    Unicode::snprintf(stepsValueBuffer, STEPSVALUE_SIZE, "%u", steps);
    stepsValue.invalidate();
}

void TrackLapView::setDistance(float m)
{
    if (mGpsFix || m > 0.001) {
        if (mUnitsImperial) {
            Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", App::Utils::km2mi(m / 1000.0f)); // mi
        } else {
            Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", m / 1000.0f);  // km
        }
    } else {
        Unicode::snprintf(distanceValueBuffer, DISTANCEVALUE_SIZE, "---");  // km
    }

    distanceValue.invalidate();
}

void TrackLapView::setTimer(std::time_t sec)
{
    uint16_t hh = 0;
    uint8_t mm = 0;
    uint8_t ss = 0;

    App::Utils::sec2hms(sec, hh, mm, ss);

    if (hh == 0) {
        Unicode::snprintf(timeValueBuffer, TIMEVALUE_SIZE, "%d:%02d", mm, ss);
    } else {
        Unicode::snprintf(timeValueBuffer, TIMEVALUE_SIZE, "%d:%02d", hh, mm);
    }

    timeValue.invalidate();
}

void TrackLapView::setGpsFix(bool state)
{
    mGpsFix = state;
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
