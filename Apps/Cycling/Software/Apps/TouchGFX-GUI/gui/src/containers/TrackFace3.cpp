#include <gui/containers/TrackFace3.hpp>

TrackFace3::TrackFace3()
{

}

void TrackFace3::initialize()
{
    TrackFace3Base::initialize();
}

void TrackFace3::setSpeed(float mps, bool isImperial, bool gpsFix)
{
    if (gpsFix || mps > 0.001f) {

        float v = (3.6f * mps); // km/h

        if (isImperial) {
            v = App::Utils::km2mi(v); // mi/h
        }
        if (v < 100.0f) {
            Unicode::snprintfFloat(speedValueBuffer, SPEEDVALUE_SIZE, "%.1f", v);
        } else {
            Unicode::snprintfFloat(speedValueBuffer, SPEEDVALUE_SIZE, "%.0f", v);
        }
    } else {
        Unicode::snprintf(speedValueBuffer, SPEEDVALUE_SIZE, "---");
    }

    if (isImperial) {
        Unicode::snprintf(speedUnitsBuffer, SPEEDUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_MI_PER_H).getText());
    } else {
        Unicode::snprintf(speedUnitsBuffer, SPEEDUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_KM_PER_H).getText());
    }

    speedValue.invalidate();
    speedUnits.invalidate();
}

void TrackFace3::setDistance(float m, bool isImperial, bool gpsFix)
{
    if (gpsFix || m > 0.001f) {

        float v = m / 1000.0f;

        if (isImperial) {
            v = App::Utils::km2mi(m / 1000.0f); // mi
        }

        if (v < 100.0f) {
            Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", v);
        } else {
            Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.01f", v);
        }
    } else {
        Unicode::snprintf(distanceValueBuffer, DISTANCEVALUE_SIZE, "---");
    }

    if (isImperial) {
        Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_MI_DOT).getText());
    } else {
        Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_KM).getText());
    }

    distanceValue.invalidate();
    distanceUnits.invalidate();
}

void TrackFace3::setTimer(std::time_t sec)
{
    uint16_t hh = 0;
    uint8_t mm = 0;
    uint8_t ss = 0;

    App::Utils::sec2hms(sec, hh, mm, ss);
    if (hh == 0) {
        Unicode::snprintf(timerValueBuffer, TIMERVALUE_SIZE, "%u:%02u", mm, ss);
    } else {
        Unicode::snprintf(timerValueBuffer, TIMERVALUE_SIZE, "%u:%02u", hh, mm);
    }
    timerValue.invalidate();
}
