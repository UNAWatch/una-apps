#include <gui/containers/TrackFace3.hpp>

TrackFace3::TrackFace3()
{

}

void TrackFace3::initialize()
{
    TrackFace3Base::initialize();
}

void TrackFace3::setLapPace(float spm, bool isImperial, bool gpsFix)
{
    if (gpsFix || spm > 0.001f) {

        std::time_t v = static_cast<std::time_t>(spm * 1000.0f); // sec/km

        if (isImperial) {
            v = static_cast<std::time_t>(v / App::Utils::km2mi(1.0f)); // sec/mi
        }

        Unicode::snprintf(lapPaceValueBuffer, LAPPACEVALUE_SIZE, "%u:%02u", App::Utils::sec2hmsM(v), App::Utils::sec2hmsS(v));
    } else {
        Unicode::snprintf(lapPaceValueBuffer, LAPPACEVALUE_SIZE, "---");
    }

    lapPaceValue.invalidate();
}

void TrackFace3::setLapDistance(float m, bool isImperial, bool gpsFix)
{
    if (gpsFix || m > 0.001f) {

        float v = m / 1000.0f; // km

        if (isImperial) {
            v = App::Utils::km2mi(m / 1000.0f); // mi
        }

        if (v < 100.0f) {
            Unicode::snprintfFloat(lapDistanceValueBuffer, LAPDISTANCEVALUE_SIZE, "%.02f", v);
        } else {
            Unicode::snprintfFloat(lapDistanceValueBuffer, LAPDISTANCEVALUE_SIZE, "%.01f", v);
        }
    } else {
        Unicode::snprintf(lapDistanceValueBuffer, LAPDISTANCEVALUE_SIZE, "---");
    }

    lapDistanceValue.invalidate();
}

void TrackFace3::setLapTimer(std::time_t sec)
{
    uint16_t hh = 0;
    uint8_t mm = 0;
    uint8_t ss = 0;

    App::Utils::sec2hms(sec, hh, mm, ss);
    if (hh == 0) {
        Unicode::snprintf(timerValueBuffer, TIMERVALUE_SIZE, "%u:%02u", mm, ss);
    } else {
        Unicode::snprintf(timerValueBuffer, TIMERVALUE_SIZE, "%u:%02u", hh, mm);
    }
    timerValue.invalidate();
}
