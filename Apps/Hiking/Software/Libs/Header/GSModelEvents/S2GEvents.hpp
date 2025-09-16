#ifndef __S2GEVENTS_HPP__
#define __S2GEVENTS_HPP__

#include <cstdint>
#include <variant>
#include <string>
#include <memory>

// Application types
#include "Settings.hpp"
#include "TrackInfo.hpp"
#include "ActivitySummary.hpp"

namespace S2GEvent
{

struct Time {
    std::tm localTime;
};

struct SettingsUpd {
    // Application settings
    Settings settings;

    // Kernel settings
    bool unitsImperial;
    std::array<uint8_t, 4> hrThresholds;
};

struct Battery {
    uint8_t level;
};

struct GpsFix {
    bool state;
};

struct TrackStateUpd {
    Track::State state;
};

struct TrackDataUpd {
    Track::Data data;
};

struct LapEnded {
    uint32_t lapNum;
};

struct Summary {
    std::shared_ptr<const ActivitySummary> summary;
};

using Data = std::variant<
        Time,
        SettingsUpd,
        Battery,
        GpsFix,
        TrackStateUpd,
        TrackDataUpd,
        LapEnded,
        Summary
>;

} // namespace S2GEvent


class IGUIModelHandler {
public:
    virtual ~IGUIModelHandler() = default;


    virtual void handleEvent(const S2GEvent::Time& event) = 0;
    virtual void handleEvent(const S2GEvent::SettingsUpd& event) = 0;
    virtual void handleEvent(const S2GEvent::Battery& event) = 0;
    virtual void handleEvent(const S2GEvent::GpsFix& event) = 0;
    virtual void handleEvent(const S2GEvent::TrackStateUpd& event) = 0;
    virtual void handleEvent(const S2GEvent::TrackDataUpd& event) = 0;
    virtual void handleEvent(const S2GEvent::LapEnded& event) = 0;
    virtual void handleEvent(const S2GEvent::Summary& event) = 0;

};

#endif
