#include <gui/containers/TrackFace1.hpp>

TrackFace1::TrackFace1()
{

}

void TrackFace1::initialize()
{
    TrackFace1Base::initialize();
}


void TrackFace1::setPace(float spm, bool isImperial, bool gpsFix)
{
    if (gpsFix || spm > 0) {

        std::time_t v = static_cast<std::time_t>(spm * 1000.0f); // sec/km

        if (isImperial) {
            v = static_cast<std::time_t>(v / App::Utils::km2mi(1.0f)); // sec/mi
        }

        Unicode::snprintf(paceValueBuffer, PACEVALUE_SIZE, "%u:%02u", App::Utils::sec2hmsM(v), App::Utils::sec2hmsS(v));
    } else {
        Unicode::snprintf(paceValueBuffer, PACEVALUE_SIZE, "---");
    }

    paceValue.invalidate();
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
