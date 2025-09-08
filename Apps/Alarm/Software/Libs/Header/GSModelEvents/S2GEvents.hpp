#ifndef __S2GEVENTS_HPP__
#define __S2GEVENTS_HPP__

#include "AppTypes.hpp"

#include <stdint.h>

#include <variant>

namespace S2GEvent {
    // Updated alarm list
    struct AlarmList {
        std::vector<AppType::Alarm> list;
    };

    // Activated alarm
    struct Alarm {
        AppType::Alarm alarm;
    };

    // Define the data types that can be sent from Service to GUI
    using Data = std::variant<
        AlarmList,
        Alarm
    >;
};

class IGUIModelHandler {
public:
    virtual ~IGUIModelHandler() = default;

    virtual void handleEvent(const S2GEvent::AlarmList& event) = 0;
    virtual void handleEvent(const S2GEvent::Alarm& event) = 0;
};

#endif
