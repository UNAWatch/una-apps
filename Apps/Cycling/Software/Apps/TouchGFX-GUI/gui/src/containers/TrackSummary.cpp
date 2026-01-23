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
    float v = m / 1000.0f;

    if (isImperial) {
        v = App::Utils::km2mi(m / 1000.0f); // mi
    }

    if (v < 100.0f) {
        Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.02f", v);
    } else {
        Unicode::snprintfFloat(distanceValueBuffer, DISTANCEVALUE_SIZE, "%.01f", v);
    }

    if (isImperial) {
        Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_MI_DOT).getText());
    } else {
        Unicode::snprintf(distanceUnitsBuffer, DISTANCEUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_KM).getText());
    }

    distanceValue.invalidate();
    distanceUnits.invalidate();
}

void TrackSummary::setAvgSpeed(float mps, bool isImperial)
{
    float v = (3.6f * mps); // km/h

    if (isImperial) {
        v = App::Utils::km2mi(v); // mi/h
    }

    if (v < 100.0f) {
        Unicode::snprintfFloat(avgSpeedValueBuffer, AVGSPEEDVALUE_SIZE, "%.1f", v);
    } else {
        Unicode::snprintfFloat(avgSpeedValueBuffer, AVGSPEEDVALUE_SIZE, "%.0f", v);
    }

    avgSpeedValue.invalidate();
}

void TrackSummary::setElevation(float m, bool isImperial)
{
    if (isImperial) {
        m = App::Utils::m2ft(m); // ft
    }
    Unicode::snprintfFloat(elevationValueBuffer, ELEVATIONVALUE_SIZE, "%.0f", m);
    elevationValue.invalidate();
    elevationText.invalidate();
}

void TrackSummary::setTimer(uint32_t sec)
{
    Unicode::snprintf(timerValueBuffer, TIMERVALUE_SIZE, "%u:%02u:%02u",
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
