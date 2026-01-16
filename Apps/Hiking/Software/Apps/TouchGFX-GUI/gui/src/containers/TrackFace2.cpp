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
#if 0   // debug option
    Unicode::snprintfFloat(hrTextBuffer, HRTEXT_SIZE, "%.0f", tl);
    hrText.invalidate();
    if (tl >= 1.0 && hr >= 1.0) {
        Unicode::snprintfFloat(hrValueBuffer, HRVALUE_SIZE, "%.0f", hr);
    } else {
        Unicode::snprintf(hrValueBuffer, HRVALUE_SIZE, "--");
    }
    hrValue.invalidate();
#else
    Unicode::snprintfFloat(hrValueBuffer, HRVALUE_SIZE, "%.0f", hr);
    hrValue.invalidate();
#endif


    hrBar.setHR(hr, th);
}

void TrackFace2::setAvgPace(float spm, bool isImperial, bool gpsFix)
{
    if (gpsFix || spm > 0.001) {
        std::time_t secPerKm = static_cast<std::time_t>(spm * 1000.0f);

        if (isImperial) {
            secPerKm = static_cast<std::time_t>(secPerKm / App::Utils::km2mi(1.0f));
        }

        Unicode::snprintf(avgPaceValueBuffer, AVGPACEVALUE_SIZE, "%d:%02d", App::Utils::sec2hmsM(secPerKm), App::Utils::sec2hmsS(secPerKm));
    } else {
        Unicode::snprintf(avgPaceValueBuffer, AVGPACEVALUE_SIZE, "---");
    }

    avgPaceValue.invalidate();
}

void TrackFace2::setElevation(float elevation, bool isImperial)
{
    if (isImperial) {
        elevation = App::Utils::m2ft(elevation);
    }
    Unicode::snprintfFloat(elevationValueBuffer, ELEVATIONVALUE_SIZE, "%.0f", elevation);
    elevationValue.invalidate();
}
