#include <gui/containers/TrackFace3.hpp>

TrackFace3::TrackFace3()
{

}

void TrackFace3::initialize()
{
    TrackFace3Base::initialize();
}

void TrackFace3::setLapPace(int32_t sec, bool isImperial)
{
    if (isImperial) {
        sec = static_cast<std::time_t>(sec / App::Utils::km2mi(1.0f));
    }
    Unicode::snprintf(lapPaceValueBuffer, LAPPACEVALUE_SIZE, "%d:%02d", App::Utils::sec2hmsM(sec), App::Utils::sec2hmsS(sec));
    lapPaceValue.invalidate();
}

void TrackFace3::setLapDistance(float km, bool isImperial)
{
    if (isImperial) {
        Unicode::snprintfFloat(lapDistanceValueBuffer, LAPDISTANCEVALUE_SIZE, "%.2f", App::Utils::km2mi(km));
    } else {
        Unicode::snprintfFloat(lapDistanceValueBuffer, LAPDISTANCEVALUE_SIZE, "%.2f", km);
    }
    lapDistanceValue.invalidate();
}

void TrackFace3::setLapTimer(std::time_t sec)
{
    Unicode::snprintf(lapTimeValueBuffer, LAPTIMEVALUE_SIZE, "%d:%02d:%02d",
        App::Utils::sec2hmsH(sec), App::Utils::sec2hmsM(sec), App::Utils::sec2hmsS(sec));
    lapTimeValue.invalidate();
}
