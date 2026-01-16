#include <gui/containers/TrackFace2.hpp>

TrackFace2::TrackFace2()
{

}

void TrackFace2::initialize()
{
    TrackFace2Base::initialize();
}

void TrackFace2::setHR(float hr, float tl, const std::array<uint8_t, 4>& th)
{
    Unicode::snprintfFloat(hrValueBuffer, HRVALUE_SIZE, "%.0f", hr);
    hrValue.invalidate();
    hrBar.setHR(hr, th);
}

void TrackFace2::setSpeed(float mps, bool isImperial, bool gpsFix)
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

void TrackFace2::setElevation(float m, bool isImperial)
{
    if (isImperial) {
        m = App::Utils::m2ft(m);
        Unicode::snprintfFloat(elevationValueBuffer, ELEVATIONVALUE_SIZE, "%.0f", App::Utils::m2ft(m));
        Unicode::snprintf(elevationUnitsBuffer, ELEVATIONUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_FT).getText());
    } else {
        Unicode::snprintfFloat(elevationValueBuffer, ELEVATIONVALUE_SIZE, "%.0f", m);
        Unicode::snprintf(elevationUnitsBuffer, ELEVATIONUNITS_SIZE, "%s", touchgfx::TypedText(T_TEXT_M).getText());
    }

    elevationValue.invalidate();
    elevationUnits.invalidate();
}

