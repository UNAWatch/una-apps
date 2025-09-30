#include <gui/containers/TrackSummary.hpp>

TrackSummary::TrackSummary()
{
    track.setPosition(kBaseTrackX, kBaseTrackY, 145, 145);
    track.setLineWidth(3);
    track.setColor(Gui::Config::Color::YELLOW_DARK);
    insert(nullptr, track); // insert to back
}

void TrackSummary::initialize()
{
    TrackSummaryBase::initialize();
}

void TrackSummary::setDistance(float km, bool isImperial)
{
    if (isImperial) {
        Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", App::Utils::km2mi(km));
        Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_MI_DOT).getText());
    } else {
        Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", km);
        Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_KM).getText());
    }
    distanceValue.invalidate();
    distanceUnits.invalidate();
}

void TrackSummary::setSteps(uint32_t v)
{
    Unicode::snprintf(stepsValueBuffer, STEPSVALUE_SIZE, "%d", v);
    stepsValue.invalidate();
    stepsText.invalidate();

}

void TrackSummary::setElevation(int32_t m, bool isImperial)
{
    if (isImperial) {
        m = static_cast<int32_t>(App::Utils::m2ft(static_cast<float>(m)));
    }
    Unicode::snprintf(elevationValueBuffer, ELEVATIONVALUE_SIZE, "%d", m);
    elevationValue.invalidate();
    elevationText.invalidate();
}

void TrackSummary::setTimer(uint32_t sec)
{
    Unicode::snprintf(timerValueBuffer, TIMERVALUE_SIZE, "%d:%02d:%02d",
        App::Utils::sec2hmsH(sec), App::Utils::sec2hmsM(sec), App::Utils::sec2hmsS(sec));
    timerValue.invalidate();
    timerText.invalidate();
}

void TrackSummary::setMap(const SDK::TrackMapScreen &map)
{
    // Move track to top left corner
    int16_t x = kBaseTrackX - map.minx;
    int16_t y = kBaseTrackY - map.miny;
    track.setXY(x, y);

    const uint8_t *points = reinterpret_cast<const uint8_t *>(map.points.data());
    track.setPoints(points, map.points.size());
}