#include <gui/containers/TrackFaceLap.hpp>
#include <SDK/Utils/Utils.hpp>

TrackFaceLap::TrackFaceLap()
{
}

void TrackFaceLap::initialize()
{
    TrackFaceLapBase::initialize();
}

void TrackFaceLap::setHR(float hr, const uint8_t* thresholds, uint8_t thresholdCount)
{
    if (hr < App::Display::kMinHR) {
        Unicode::snprintf(hrValueBuffer, HRVALUE_SIZE, "---");
        hrValue.invalidate();
        return;
    }

    Unicode::snprintfFloat(hrValueBuffer, HRVALUE_SIZE, "%.0f", hr);
    hrValue.invalidate();

    hrZone.setHR(static_cast<uint8_t>(hr), thresholds, thresholdCount);
}

void TrackFaceLap::setPace(float pace)
{
    if (pace < App::Display::kMinPace) {
        Unicode::snprintf(lapPaceValueBuffer, LAPPACEVALUE_SIZE, "---");
    } else {
        auto hms = SDK::Utils::toHMS(static_cast<std::time_t>(pace));
        Unicode::snprintf(lapPaceValueBuffer, LAPPACEVALUE_SIZE, "%u:%02u", hms.m, hms.s);
    }
    lapPaceValue.invalidate();
}

void TrackFaceLap::setDistance(float dist)
{
    if (dist < App::Display::kMinDist) {
        Unicode::snprintf(lapDistValueBuffer, LAPDISTVALUE_SIZE, "---");
    } else if (dist < 100.0f) {
        Unicode::snprintfFloat(lapDistValueBuffer, LAPDISTVALUE_SIZE, "%.02f", dist);
    } else {
        Unicode::snprintfFloat(lapDistValueBuffer, LAPDISTVALUE_SIZE, "%.01f", dist);
    }
    lapDistValue.invalidate();
}

void TrackFaceLap::setTimer(std::time_t sec)
{
    auto hms = SDK::Utils::toHMS(sec);
    if (hms.h == 0) {
        Unicode::snprintf(lapTimerValueBuffer, LAPTIMERVALUE_SIZE, "%u:%02u", hms.m, hms.s);
    } else {
        Unicode::snprintf(lapTimerValueBuffer, LAPTIMERVALUE_SIZE, "%u:%02u", hms.h, hms.m);
    }
    lapTimerValue.invalidate();
}
