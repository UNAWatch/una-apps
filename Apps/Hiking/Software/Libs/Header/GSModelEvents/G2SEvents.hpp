#ifndef __G2SEVENTS_HPP__
#define __G2SEVENTS_HPP__

#include <cstdint>
#include <variant>

// Application types
#include "Settings.hpp"

namespace G2SEvent
{

    struct GuiRun {};
    struct GuiStop {};

    struct TrackStart {};
    struct TrackStop {
        bool discard;   // If true, tarck will be discarded, otherwise saved
    };
    struct SettingsSave {
        Settings settings;
    };

    struct InternalRefresh {};

    using Data = std::variant<
            GuiRun,
            GuiStop,
            TrackStart,
            TrackStop,
            SettingsSave,

            InternalRefresh
    >;

} // namespace G2SEvent


class IServiceModelHandler {
public:
    virtual ~IServiceModelHandler() = default;

    virtual void handleEvent(const G2SEvent::GuiRun& event)  = 0;
    virtual void handleEvent(const G2SEvent::GuiStop& event) = 0;
    virtual void handleEvent(const G2SEvent::TrackStart& event) = 0;
    virtual void handleEvent(const G2SEvent::TrackStop& event) = 0;
    virtual void handleEvent(const G2SEvent::SettingsSave& event) = 0;

    virtual void handleEvent(const G2SEvent::InternalRefresh& event) = 0;

};

#endif
