#include <gui/containers/TrackFace1.hpp>

TrackFace1::TrackFace1()
{

}

void TrackFace1::initialize()
{
    TrackFace1Base::initialize();
}


void TrackFace1::setSteps(int32_t v)
{
    Unicode::snprintf(stepsValueBuffer, STEPSVALUE_SIZE, "%u", v);
    stepsValue.invalidate();
}

void TrackFace1::setDistance(float km, bool isImperial)
{
    if (isImperial) {
        Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", Gui::Utils::km2mi(km));
        Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_MI_DOT).getText());
    } else {
        Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", km);
        Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_KM).getText());
    }
    distanceValue.invalidate();
}

void TrackFace1::setTimer(std::time_t sec)
{
    Unicode::snprintf(timerValueBuffer, TIMERVALUE_SIZE, "%d:%02d:%02d",
        Gui::Utils::sec2hmsH(sec), Gui::Utils::sec2hmsM(sec), Gui::Utils::sec2hmsS(sec));
    timerValue.invalidate();
}