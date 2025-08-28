#ifndef __IMODEL_HPP__
#define __IMODEL_HPP__

#include "Interfaces/IKernel.hpp"

#include <stdint.h>

#include <variant>
#include <string>

namespace S2GEvent {    // Service-to-GUI events
    enum class State {
        none,
        suspend,
        resume,
        quit
    };

    struct UpdateState {
        State state;
    };



    using Data = std::variant<
            UpdateState

    >;
};

namespace G2SEvent {    // GUI-to-Service events
    struct Run {};
    struct Stop {};

    struct InternalRefresh {};

    using Data = std::variant<
            Run,
            Stop,

            InternalRefresh
    >;
};


class IServiceModelHandler {
public:
    virtual ~IServiceModelHandler() = default;

    virtual void handleEvent(const G2SEvent::Run& event)  = 0;
    virtual void handleEvent(const G2SEvent::Stop& event) = 0;

    virtual void handleEvent(const G2SEvent::InternalRefresh& event) = 0;
};

class IGUIModelHandler {
public:
    virtual ~IGUIModelHandler() = default;

    void handleEvent(const S2GEvent::UpdateState& event)
    {
        mGUIState = event.state;
    }

    S2GEvent::State getGUIState()
    {
        S2GEvent::State res = mGUIState;

        mGUIState = S2GEvent::State::none;

        return res;
    }

    //virtual void handleEvent(const S2GEvent::Counter& event) = 0;


private:
    S2GEvent::State mGUIState;
};

class IGUIModel {
public:
    virtual ~IGUIModel() = default;

    virtual void            setGUIHandler(const IKernel* kernel, IGUIModelHandler* handler) = 0;
    virtual S2GEvent::State checkS2GEvents(uint32_t timeout = 0xFFFFFFFF)                   = 0;
    virtual bool            sendToService(const G2SEvent::Data& data)                       = 0;
    virtual void            updateGUIState(S2GEvent::State state)                           = 0;

};

#endif // __IMODEL_HPP__
