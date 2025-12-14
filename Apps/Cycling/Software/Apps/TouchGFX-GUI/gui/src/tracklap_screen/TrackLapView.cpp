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

    Unicode::snprintf(buffer, kBuffSize, "%s %u",
        touchgfx::TypedText(T_TEXT_LAP).getText(), n + 1);

    title.set(buffer);
}

void TrackLapView::setPace(float spm)
{
    std::time_t secPerKm = static_cast<std::time_t>(spm * 1000.0f);
    if (mUnitsImperial) {
        secPerKm = static_cast<std::time_t>(secPerKm / App::Utils::km2mi(1.0f));
    }
    Unicode::snprintf(avgPaceValueBuffer, AVGPACEVALUE_SIZE, "%d:%02d", App::Utils::sec2hmsM(secPerKm), App::Utils::sec2hmsS(secPerKm));
    avgPaceValue.invalidate();
}

void TrackLapView::setDistance(float m)
{
    if (mUnitsImperial) {
        Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", App::Utils::km2mi(m / 1000.0f)); // mi
    } else {
        Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", m / 1000.0f);  // km
    }

    distanceValue.invalidate();
}

void TrackLapView::setTimer(std::time_t sec)
{
    Unicode::snprintf(timeValueBuffer, TIMEVALUE_SIZE, "%d:%02d", App::Utils::sec2hmsH(sec), App::Utils::sec2hmsM(sec));
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
