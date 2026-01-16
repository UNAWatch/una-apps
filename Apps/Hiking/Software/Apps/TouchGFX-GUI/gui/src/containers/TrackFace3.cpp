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
    std::time_t sekPerKm = static_cast<std::time_t>(spm * 1000.0f);

    if (gpsFix || sekPerKm > 0) {
        if (isImperial) {
            sekPerKm = static_cast<std::time_t>(sekPerKm / App::Utils::km2mi(1.0f));
        }

        Unicode::snprintf(lapPaceValueBuffer, LAPPACEVALUE_SIZE, "%d:%02d", App::Utils::sec2hmsM(sekPerKm), App::Utils::sec2hmsS(sekPerKm));
    } else {
        Unicode::snprintf(lapPaceValueBuffer, LAPPACEVALUE_SIZE, "---");
    }

    lapPaceValue.invalidate();
}

void TrackFace3::setLapDistance(float m, bool isImperial, bool gpsFix)
{
    if (gpsFix || m > 0.001) {
        if (isImperial) {
            Unicode::snprintfFloat(lapDistanceValueBuffer, LAPDISTANCEVALUE_SIZE, "%.2f", App::Utils::km2mi(m / 1000.0f));
        } else {
            Unicode::snprintfFloat(lapDistanceValueBuffer, LAPDISTANCEVALUE_SIZE, "%.2f", m / 1000.0f);
        }
    } else {
        Unicode::snprintf(lapDistanceValueBuffer, LAPDISTANCEVALUE_SIZE, "---  ");
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
        Unicode::snprintf(lapTimeValueBuffer, LAPTIMEVALUE_SIZE, "%d:%02d", mm, ss);
    } else {
        Unicode::snprintf(lapTimeValueBuffer, LAPTIMEVALUE_SIZE, "%d:%02d", hh, mm);
    }

    lapTimeValue.invalidate();
}
