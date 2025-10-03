#ifndef __S2GEVENTS_HPP__
#define __S2GEVENTS_HPP__

#include <stdint.h>

#include <variant>

namespace S2GEvent {
    struct HeartRate {
        float heartRate;
        float trustLevel;
    };

    using Data = std::variant<
        HeartRate
    >;
};

class IGUIModelHandler {
public:
    virtual ~IGUIModelHandler() = default;

    virtual void handleEvent(const S2GEvent::HeartRate& event) = 0;
};

#endif
