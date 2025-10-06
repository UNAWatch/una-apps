#ifndef __G2SEVENTS_HPP__
#define __G2SEVENTS_HPP__

#include "AppTypes.hpp"

#include <stdint.h>

#include <variant>

namespace G2SEvent {
    // User-defined event data types

    struct AlarmSaveList {
        std::vector<AppType::Alarm> list;
    };

    struct AlarmActiveteEffect {
        AppType::Alarm alarm;
    };
    struct AlarmStop {
        AppType::Alarm alarm;
    };
    struct AlarmStopAll {};
    struct AlarmSnooze {
        AppType::Alarm alarm;
    };
    struct AlarmSnoozeAll {};


    // Define the data types that can be sent from GUI to Service
    using Data = std::variant<

        AlarmSaveList,
        AlarmActiveteEffect,
        AlarmStop,
        AlarmStopAll,
        AlarmSnooze,
        AlarmSnoozeAll

    >;
};

class IServiceModelHandler {
public:
    virtual ~IServiceModelHandler() = default;

    virtual void handleEvent(const G2SEvent::AlarmSaveList& event) = 0;
    virtual void handleEvent(const G2SEvent::AlarmActiveteEffect& event) = 0;
    virtual void handleEvent(const G2SEvent::AlarmStop& event) = 0;
    virtual void handleEvent(const G2SEvent::AlarmStopAll& event) = 0;
    virtual void handleEvent(const G2SEvent::AlarmSnooze& event) = 0;
    virtual void handleEvent(const G2SEvent::AlarmSnoozeAll& event) = 0;
};

#endif
