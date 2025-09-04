#ifndef __S2GEVENTS_HPP__
#define __S2GEVENTS_HPP__

#include <stdint.h>

#include <variant>

namespace S2GEvent {
    struct Counter {
        uint32_t value;
    };

    using Data = std::variant<
            Counter
    >;
};

class IGUIModelHandler {
public:
    virtual ~IGUIModelHandler() = default;

    virtual void handleEvent(const S2GEvent::Counter& event) = 0;
};

#endif
