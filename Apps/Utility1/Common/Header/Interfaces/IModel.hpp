#ifndef __IMODEL_TEMPLATE_HPP__
#define __IMODEL_TEMPLATE_HPP__

#include "Interfaces/IKernel.hpp"

#include <stdint.h>

#include <variant>
#include <string>

namespace S2GEvent {    // Service-to-GUI events
    struct Counter {
        uint32_t value;
    };

    using Data = std::variant<
            Counter
    >;
};

namespace G2SEvent {    // GUI-to-Service events
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

class IGUIModelHandler {
public:
    virtual ~IGUIModelHandler() = default;

    virtual void handleEvent(const S2GEvent::Counter& event) = 0;
};

class IGUIModel {
public:
    IGUIModel()
        : mGUIHandler()
        , mGUIKernel(nullptr)
    {}

    virtual ~IGUIModel() = default;

    void setGUIHandler(const IKernel* kernel, IGUIModelHandler* handler)
    {
        mGUIKernel  = kernel;
        mGUIHandler = handler;
    }

    virtual void checkS2GEvents()                          = 0;
    virtual bool sendToService(const G2SEvent::Data& data) = 0;

protected:
    IGUIModelHandler* mGUIHandler;
    const IKernel*    mGUIKernel;
};

#endif
