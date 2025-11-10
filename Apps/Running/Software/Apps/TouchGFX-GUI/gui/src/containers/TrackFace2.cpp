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

void TrackFace2::setLapPace(float spm, bool isImperial)
{
    std::time_t secPerKm = static_cast<std::time_t>(spm * 1000.0f);
    if (isImperial) {
        secPerKm = static_cast<std::time_t>(secPerKm / App::Utils::km2mi(1.0f));
    }
    Unicode::snprintf(lapPaceValueBuffer, LAPPACEVALUE_SIZE, "%d:%02d", App::Utils::sec2hmsM(secPerKm), App::Utils::sec2hmsS(secPerKm));
    lapPaceValue.invalidate();
}

void TrackFace2::setLapDistance(float m, bool isImperial)
{
    if (isImperial) {
        Unicode::snprintfFloat(lapDistValueBuffer, LAPDISTVALUE_SIZE, "%.02f", App::Utils::km2mi(m / 1000.0f)); // mi
    } else {
        Unicode::snprintfFloat(lapDistValueBuffer, LAPDISTVALUE_SIZE, "%.02f", m / 1000.0f);  // km
    }
    lapDistValue.invalidate();
}

void TrackFace2::setLapTimer(std::time_t sec)
{
    Unicode::snprintf(timerValueBuffer, TIMERVALUE_SIZE, "%u:%02u:%02u", App::Utils::sec2hmsH(sec), App::Utils::sec2hmsM(sec), App::Utils::sec2hmsS(sec));
    timerValue.invalidate();
}
