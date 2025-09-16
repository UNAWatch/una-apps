#include <gui/containers/TrackFace2.hpp>

TrackFace2::TrackFace2()
{

}

void TrackFace2::initialize()
{
    TrackFace2Base::initialize();
}

void TrackFace2::setHR(float hr)
{
    Unicode::snprintfFloat(hrValueBuffer, HRVALUE_SIZE, "%.0f", hr);
    hrValue.invalidate();

    hrBar.setHR(hr);
}

void TrackFace2::setAvgPace(int32_t sec, bool isImperial)
{
    if (isImperial) {
        sec = static_cast<std::time_t>(sec / Gui::Utils::km2mi(1.0f));
    }
    Unicode::snprintf(avgPaceValueBuffer, AVGPACEVALUE_SIZE, "%d:%02d", Gui::Utils::sec2hmsM(sec), Gui::Utils::sec2hmsS(sec));
    avgPaceValue.invalidate();
}

void TrackFace2::setElevation(float elevation, bool isImperial)
{
    if (isImperial) {
        elevation = Gui::Utils::m2ft(elevation);
    }
    Unicode::snprintf(elevationValueBuffer, ELEVATIONVALUE_SIZE, "%u", static_cast<uint32_t>(elevation));
    elevationValue.invalidate();
}
