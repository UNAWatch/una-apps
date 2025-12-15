#include <gui/containers/TrackFace1.hpp>

TrackFace1::TrackFace1()
{

}

void TrackFace1::initialize()
{
    TrackFace1Base::initialize();
}


void TrackFace1::setSpeed(float mps, bool isImperial)
{
    float kmPerH = (3.6f * mps);
    if (isImperial) {
        Unicode::snprintfFloat(speedValueBuffer, SPEEDVALUE_SIZE, "%.1f", App::Utils::km2mi(kmPerH)); // mi
        Unicode::snprintf(speedUnitsBuffer, SPEEDUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_MI_PER_H).getText());
    } else {
        Unicode::snprintfFloat(speedValueBuffer, SPEEDVALUE_SIZE, "%.1f", kmPerH);
        Unicode::snprintf(speedUnitsBuffer, SPEEDUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_KM_PER_H).getText());
    }

    speedValue.invalidate();
    speedUnits.invalidate();
}

void TrackFace1::setDistance(float m, bool isImperial)
{
    if (isImperial) {
        Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", App::Utils::km2mi(m / 1000.0f)); // mi
        Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_MI_DOT).getText());
    } else {
        Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", m / 1000.0f);  // km
        Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_KM).getText());
    }
    distanceValue.invalidate();
}

void TrackFace1::setTimer(std::time_t sec)
{
    Unicode::snprintf(timerValueBuffer, TIMERVALUE_SIZE, "%u:%02u:%02u", App::Utils::sec2hmsH(sec), App::Utils::sec2hmsM(sec), App::Utils::sec2hmsS(sec));
    timerValue.invalidate();
}
