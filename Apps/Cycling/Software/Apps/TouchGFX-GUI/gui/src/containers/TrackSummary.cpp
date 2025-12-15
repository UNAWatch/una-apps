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

void TrackSummary::setDistance(float m, bool isImperial)
{
    if (isImperial) {
        Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", App::Utils::km2mi(m / 1000.0f)); // mi
        Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_MI_DOT).getText());
    } else {
        Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", m / 1000.0f); // km
        Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_KM).getText());
    }
    distanceValue.invalidate();
    distanceUnits.invalidate();
}

void TrackSummary::setAvgSpeed(float mps, bool isImperial)
{
    float kmPerH = (3.6f * mps);
    if (isImperial) {
        Unicode::snprintfFloat(avgSpeedValueBuffer, AVGSPEEDVALUE_SIZE, "%.1f", App::Utils::km2mi(kmPerH)); // mi
    } else {
        Unicode::snprintfFloat(avgSpeedValueBuffer, AVGSPEEDVALUE_SIZE, "%.1f", kmPerH);
    }

    avgSpeedValue.invalidate();
}

void TrackSummary::setElevation(float m, bool isImperial)
{
    if (isImperial) {
        m = App::Utils::m2ft(m);
    }
    Unicode::snprintfFloat(elevationValueBuffer, ELEVATIONVALUE_SIZE, "%.0f", m);
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
