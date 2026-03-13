#include <gui/containers/TrackFace2.hpp>

TrackFace2::TrackFace2()
{

}

void TrackFace2::initialize()
{
    TrackFace2Base::initialize();
}

void TrackFace2::setHR(float hr, float tl, const std::array<uint8_t, kHrThresholdsCount>& th)
{
    Unicode::snprintfFloat(hrValueBuffer, HRVALUE_SIZE, "%.0f", hr);
    hrValue.invalidate();
    hrBar.setHR(hr, th.data(), kHrThresholdsCount);
}

void TrackFace2::setAvgPace(float spm, bool isImperial, bool gpsFix)
{
    if (gpsFix || spm > 0.001f) {

        std::time_t v = static_cast<std::time_t>(spm * 1000.0f); // sec/km

        if (isImperial) {
            v = static_cast<std::time_t>(v / App::Utils::km2mi(1.0f)); // sec/mi
        }

        Unicode::snprintf(avgPaceValueBuffer, AVGPACEVALUE_SIZE, "%u:%02u", App::Utils::sec2hmsM(v), App::Utils::sec2hmsS(v));
    } else {
        Unicode::snprintf(avgPaceValueBuffer, AVGPACEVALUE_SIZE, "---");
    }

    avgPaceValue.invalidate();
}

void TrackFace2::setElevation(float m, bool isImperial)
{
    if (isImperial) {
        m = App::Utils::m2ft(m); // ft
        Unicode::snprintf(elevationUnitsBuffer, ELEVATIONUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_FT).getText());
    } else {
        Unicode::snprintf(elevationUnitsBuffer, ELEVATIONUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_M).getText());
    }

    Unicode::snprintfFloat(elevationValueBuffer, ELEVATIONVALUE_SIZE, "%.0f", m);

    elevationValue.invalidate();
    elevationUnits.invalidate();
}
