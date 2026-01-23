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

void TrackSummary::setAvgPace(float spm, bool isImperial)
{
    std::time_t v = static_cast<std::time_t>(spm * 1000.0f); // sec/km

    if (isImperial) {
        v = static_cast<std::time_t>(v / App::Utils::km2mi(1.0f)); // sec/mi
    }
    Unicode::snprintf(avgPaceValueBuffer, AVGPACEVALUE_SIZE, "%u:%02u", App::Utils::sec2hmsM(v), App::Utils::sec2hmsS(v));
    avgPaceValue.invalidate();
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
