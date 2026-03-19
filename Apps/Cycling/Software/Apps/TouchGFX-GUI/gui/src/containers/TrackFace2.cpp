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

void TrackFace2::setSpeed(float mps, bool isImperial, bool gpsFix)
{
    if (gpsFix || mps > 0.001f) {

        float v = (3.6f * mps); // km

        if (isImperial) {
            v = App::Utils::km2mi(v); // mi
        }

        if (v < 100.0f) {
            Unicode::snprintfFloat(speedValueBuffer, SPEEDVALUE_SIZE, "%.1f", v);
        } else {
            Unicode::snprintfFloat(speedValueBuffer, SPEEDVALUE_SIZE, "%.0f", v);
        }
    } else {
        Unicode::snprintf(speedValueBuffer, SPEEDVALUE_SIZE, "---");
    }

    if (isImperial) {
        Unicode::snprintf(speedUnitsBuffer, SPEEDUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_MI_PER_H).getText());
    } else {
        Unicode::snprintf(speedUnitsBuffer, SPEEDUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_KM_PER_H).getText());
    }

    speedValue.invalidate();
    speedUnits.invalidate();
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

