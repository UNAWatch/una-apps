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
    std::time_t secPerKm = static_cast<std::time_t>(spm * 1000.0f);

    if (gpsFix || secPerKm > 0) {
        if (isImperial) {
            secPerKm = static_cast<std::time_t>(secPerKm / App::Utils::km2mi(1.0f));
        }

        Unicode::snprintf(paceValueBuffer, PACEVALUE_SIZE, "%d:%02d", App::Utils::sec2hmsM(secPerKm), App::Utils::sec2hmsS(secPerKm));
    } else {
        Unicode::snprintf(paceValueBuffer, PACEVALUE_SIZE, "---");
    }

    paceValue.invalidate();
}

void TrackFace1::setDistance(float m, bool isImperial, bool gpsFix)
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

void TrackFace1::setTimer(std::time_t sec)
{
    Unicode::snprintf(timerValueBuffer, TIMERVALUE_SIZE, "%u:%02u:%02u", App::Utils::sec2hmsH(sec), App::Utils::sec2hmsM(sec), App::Utils::sec2hmsS(sec));
    timerValue.invalidate();
}
