#include <gui/containers/TrackFace1.hpp>

TrackFace1::TrackFace1()
{

}

void TrackFace1::initialize()
{
    TrackFace1Base::initialize();
}


void TrackFace1::setSpeed(float mps, bool isImperial, bool gpsFix)
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

void TrackFace1::setDistance(float m, bool isImperial, bool gpsFix)
{
    if (gpsFix || m > 0.001f) {

        float v = m / 1000.0f; // km

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

void TrackFace1::setTimer(std::time_t sec)
{
    Unicode::snprintf(timerValueBuffer, TIMERVALUE_SIZE, "%u:%02u:%02u", App::Utils::sec2hmsH(sec), App::Utils::sec2hmsM(sec), App::Utils::sec2hmsS(sec));
    timerValue.invalidate();
}
