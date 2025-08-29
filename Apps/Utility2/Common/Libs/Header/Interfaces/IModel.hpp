#ifndef __IMODEL_HPP__
#define __IMODEL_HPP__

#include "Interfaces/IKernel.hpp"

#include <stdint.h>

#include <variant>
#include <string>

//////////////////////////////////////////////////////////
//// Service-to-GUI events
//////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////
//// GUI-to-Service events
//////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////
//// IGUIModel
//////////////////////////////////////////////////////////

class IGUIModel {
public:
    virtual ~IGUIModel() = default;

    virtual void            setGUIHandler(const IKernel* kernel, IGUIModelHandler* handler) = 0;
    virtual S2GEvent::State checkS2GEvents(uint32_t timeout = 0xFFFFFFFF)                   = 0;
    virtual bool            sendToService(const G2SEvent::Data& data)                       = 0;
    virtual void            updateGUIState(S2GEvent::State state)                           = 0;

};

#endif // __IMODEL_HPP__
