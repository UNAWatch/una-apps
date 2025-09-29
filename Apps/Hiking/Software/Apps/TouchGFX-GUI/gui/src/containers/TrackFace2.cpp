#include <gui/containers/TrackFace2.hpp>

TrackFace2::TrackFace2()
{

}

void TrackFace2::initialize()
{
    TrackFace2Base::initialize();
}

void TrackFace2::setHR(float hr, const std::array<uint8_t, 4>& th)
{
    Unicode::snprintfFloat(hrValueBuffer, HRVALUE_SIZE, "%.0f", hr);
    hrValue.invalidate();

    hrBar.setHR(hr, th);
}

void TrackFace2::setAvgPace(int32_t sec, bool isImperial)
{
    if (isImperial) {
        sec = static_cast<std::time_t>(sec / App::Utils::km2mi(1.0f));
    }
    Unicode::snprintf(avgPaceValueBuffer, AVGPACEVALUE_SIZE, "%d:%02d", App::Utils::sec2hmsM(sec), App::Utils::sec2hmsS(sec));
    avgPaceValue.invalidate();
}

void TrackFace2::setElevation(float elevation, bool isImperial)
{
    if (isImperial) {
        elevation = App::Utils::m2ft(elevation);
    }
    Unicode::snprintf(elevationValueBuffer, ELEVATIONVALUE_SIZE, "%u", static_cast<uint32_t>(elevation));
    elevationValue.invalidate();
}
