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
    float kmPerH = (3.6f * mps);

    if (gpsFix || kmPerH > 0.001f) {
        if (isImperial) {
            Unicode::snprintfFloat(speedValueBuffer, SPEEDVALUE_SIZE, "%.1f", App::Utils::km2mi(kmPerH)); // mi
            Unicode::snprintf(speedUnitsBuffer, SPEEDUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_MI_PER_H).getText());
        } else {
            Unicode::snprintfFloat(speedValueBuffer, SPEEDVALUE_SIZE, "%.1f", kmPerH);
            Unicode::snprintf(speedUnitsBuffer, SPEEDUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_KM_PER_H).getText());
        }
    } else {
        Unicode::snprintf(speedValueBuffer, SPEEDVALUE_SIZE, "---");
    }

    speedValue.invalidate();
    speedUnits.invalidate();
}

void TrackFace3::setDistance(float m, bool isImperial, bool gpsFix)
{
    if (gpsFix || m > 0.001f) {
        if (isImperial) {
            Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", App::Utils::km2mi(m / 1000.0f)); // mi
            Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_MI_DOT).getText());
        } else {
            Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", m / 1000.0f);  // km
            Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_KM).getText());
        }
    } else {
        Unicode::snprintf(distanceValueBuffer, DISTANCEVALUE_SIZE, "---  ");
    }

    distanceValue.invalidate();
}

void TrackFace3::setTimer(std::time_t sec)
{
    uint16_t hh = 0;
    uint8_t mm = 0;
    uint8_t ss = 0;

    App::Utils::sec2hms(sec, hh, mm, ss);
    if (hh == 0) {
        Unicode::snprintf(timerValueBuffer, TIMERVALUE_SIZE, "%d:%02d", mm, ss);
    } else {
        Unicode::snprintf(timerValueBuffer, TIMERVALUE_SIZE, "%d:%02d", hh, mm);
    }
    timerValue.invalidate();
}
