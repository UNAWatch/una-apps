#ifndef __G2SEVENTS_HPP__
#define __G2SEVENTS_HPP__

#include <stdint.h>

#include <variant>

namespace G2SEvent {
    struct Dummy {};    // Change it to your event

    using Data = std::variant<
            Dummy
    >;
};

class IServiceModelHandler {
public:
    virtual ~IServiceModelHandler() = default;

    virtual void handleEvent(const G2SEvent::Dummy& event) {};
};

#endif
