#include <gui/track_screen/TrackView.hpp>

TrackView::TrackView()
{

}

void TrackView::setupScreen()
{
    TrackViewBase::setupScreen();

    buttons.setL1(Buttons::NONE);
    buttons.setL2(Buttons::NONE);
    buttons.setR1(Buttons::NONE);
    buttons.setR2(Buttons::NONE);

    sideBar.setCount(App::Menu::Start::Track::ID_COUNT);
}

void TrackView::tearDownScreen()
{
    TrackViewBase::tearDownScreen();
}


void TrackView::setPositionId(uint16_t id)
{
    if (id >= App::Menu::Start::Track::ID_COUNT) {
        return;
    }

    sideBar.setActiveId(id);

    trackFace1.setVisible(false);
    trackFace2.setVisible(false);
    trackFace3.setVisible(false);
    trackFace4.setVisible(false);

    switch (id) {
        case App::Menu::Start::Track::ID_TRACK4:
            trackFace4.setVisible(true);
            break;
        case App::Menu::Start::Track::ID_TRACK3:
            trackFace3.setVisible(true);
            break;
        case App::Menu::Start::Track::ID_TRACK2:
            trackFace2.setVisible(true);
            break;
        default:    // App::Menu::Start::Track::TRACK1:
            trackFace1.setVisible(true);
            break;
    }

    trackFace1.invalidate();
    trackFace2.invalidate();
    trackFace3.invalidate();
    trackFace4.invalidate();
}

uint16_t TrackView::getPositionId()
{
    return sideBar.getActiveId();
}

void TrackView::setTrackData(const Track::Data &data, bool isImperial, const std::array<uint8_t, kHrThresholdsCount>& hrth)
{
    trackFace1.setSpeed(data.speed, isImperial, mGpsFix);
    trackFace1.setDistance(data.distance, isImperial, mGpsFix);
    trackFace1.setTimer(data.totalTime);

    trackFace2.setHR(data.HR, data.hrTrustLevel, hrth);
    trackFace2.setSpeed(data.avgSpeed, isImperial, mGpsFix);
    trackFace2.setElevation(data.elevation, isImperial);

    trackFace3.setSpeed(data.avgLapSpeed, isImperial, mGpsFix);
    trackFace3.setDistance(data.lapDistance, isImperial, mGpsFix);
    trackFace3.setTimer(data.lapTime);
}

void TrackView::setTime(uint8_t h, uint8_t m)
{
    trackFace4.setTime(h, m);
}

void TrackView::setBatteryLevel(uint8_t level)
{
    trackFace4.setBatteryLevel(level);
}

void TrackView::setCharging(bool state)
{

}

void TrackView::handleKeyEvent(uint8_t key)
{
    if (key == Gui::Config::Button::L1) {
        uint16_t p = getPositionId();
        if (p == 0) {
            p = App::Menu::Start::Track::ID_COUNT;
        }
        p--;
        setPositionId(p);
    }

    if (key == Gui::Config::Button::L2) {
        uint16_t p = getPositionId();
        p++;
        if (p == App::Menu::Start::Track::ID_COUNT) {
            p = 0;
        }
        setPositionId(p);
    }

    if (key == Gui::Config::Button::R1) {
        application().gotoTrackActionScreenNoTransition();
    }

    if (key == Gui::Config::Button::R2) {
        presenter->saveLap();
        application().gotoTrackLapScreenNoTransition();
    }
}

void TrackView::setGpsFix(bool state)
{
    mGpsFix = state;
}

void TrackView::handleTickEvent()
{
    if (mGpsFix && !gpsDot.isVisible()) {
        mGpsFixBlinkCounter = 0;
        return;
    }

    if (mGpsFixBlinkCounter > GUI_CONFIG_MS_2_TICKS(500)) {
        if (mGpsFix) {
            gpsDot.setVisible(false);
        } else {
            gpsDot.setVisible(!gpsDot.isVisible());
        }
        gpsDot.invalidate();
        mGpsFixBlinkCounter = 0;
    }

    mGpsFixBlinkCounter++;
}
