#ifndef __G2SEVENTS_HPP__
#define __G2SEVENTS_HPP__

#include <stdint.h>

#include <variant>

namespace G2SEvent {
    struct Run {};
    struct Stop {};

    using Data = std::variant<
            Run,
            Stop
    >;
};

class IServiceModelHandler {
public:
    virtual ~IServiceModelHandler() = default;

    virtual void handleEvent(const G2SEvent::Run& event)  = 0;
    virtual void handleEvent(const G2SEvent::Stop& event) = 0;
};

#endif
