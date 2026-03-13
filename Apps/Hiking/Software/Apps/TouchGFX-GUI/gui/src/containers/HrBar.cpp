#include <gui/containers/HrBar.hpp>

HrBar::HrBar()
    : mZones{&hr1, &hr2, &hr3, &hr4, &hr5}
{
}

void HrBar::initialize()
{
    HrBarBase::initialize();
}

void HrBar::setHR(float hr, const uint8_t* th, uint8_t size)
{
    if (th == nullptr || size == 0) {
        return;
    }

    if (size > 5) {
        size = 5;
    }

    for (auto zone : mZones) {
        zone->setVisible(false);
    }

    int activeZone = -1;
    for (int i = size - 1; i >= 0; --i) {
        if (hr > th[i]) {
            // The threshold th[i] corresponds to the zone:
            // size=5: i=0 -> zone 0, i=4 -> zone 4
            // size=4: i=0 -> zone 1, i=3 -> zone 4
            activeZone = static_cast<int>(mZones.size() - size + i);
            break;
        }
    }

    // If nothing is found:
    // - if size == 5 -> show nothing
    // - if size < 5 -> enable the last unverified zone
    if (activeZone < 0 && size < mZones.size()) {
        activeZone = static_cast<int>(mZones.size() - size - 1);
    }

    if (activeZone >= 0) {
        mZones[activeZone]->setVisible(true);
    }

    for (auto zone : mZones) {
        zone->invalidate();
    }
}
