#ifndef __IMODEL_HPP__
#define __IMODEL_HPP__

#include "SDK/Interfaces/IKernel.hpp"

#include <cstdint>

#include <variant>
#include <string>
#include <memory>

#include "AppTypes.hpp"

/**
 * @brief   Data types to send Service --> GUI
 */
namespace S2GEvent {

enum class State {
    none,
    suspend,
    resume,
    quit
};

struct UpdateState {
    State state;
};


// User-defined event data types



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
    UpdateState,

    AlarmList,
    Alarm
>;

};

/**
 * @brief   Data types to send GUI --> Service
 */
namespace G2SEvent {
struct Run {};
struct Stop {};

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


struct InternalRefresh {};


// Define the data types that can be sent from GUI to Service
using Data = std::variant<
    Run,
    Stop,

    AlarmSaveList,
    AlarmActiveteEffect,
    AlarmStop,
    AlarmStopAll,
    AlarmSnooze,
    AlarmSnoozeAll,

    InternalRefresh
>;
};


class IServiceModelHandler {
public:
    virtual ~IServiceModelHandler() = default;

    virtual void handleEvent(const G2SEvent::Run& event)  = 0;
    virtual void handleEvent(const G2SEvent::Stop& event) = 0;
    virtual void handleEvent(const G2SEvent::AlarmSaveList& event) = 0;
    virtual void handleEvent(const G2SEvent::AlarmActiveteEffect& event) = 0;
    virtual void handleEvent(const G2SEvent::AlarmStop& event) = 0;
    virtual void handleEvent(const G2SEvent::AlarmStopAll& event) = 0;
    virtual void handleEvent(const G2SEvent::AlarmSnooze& event) = 0;
    virtual void handleEvent(const G2SEvent::AlarmSnoozeAll& event) = 0;
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

    // User-defined event handlers
    virtual void handleEvent(const S2GEvent::AlarmList& event) = 0;
    virtual void handleEvent(const S2GEvent::Alarm& event) = 0;

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
