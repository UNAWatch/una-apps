#ifndef __IMODEL_HPP__
#define __IMODEL_HPP__

#include "Interfaces/IKernel.hpp"

#include <stdint.h>

#include <variant>
#include <string>
#include <memory>

// Application types
#include "Common/Header/Settings.hpp"
#include "Common/Header/TrackInfo.hpp"
#include "Common/Header/ActivitySummary.hpp"

namespace S2GEvent
{    // Service-to-GUI events
enum class State {
    none,
    suspend,
    resume,
    quit
};

struct UpdateState {
    State state;
};

struct Time {
    std::tm localTime;
};

struct SettingsUpd {
    // Application settings
    Settings settings;

    // Kernel settings
    bool unitsImperial;
    std::array<uint8_t, 4> hrThresholds;
};

struct Battery {
    uint8_t level;
};

struct GpsFix {
    bool state;
};

struct TrackStateUpd {
    Track::State state;
};

struct TrackDataUpd {
    Track::Data data;
};

struct LapEnded {
    uint32_t lapNum;
};

struct Summary {
    std::shared_ptr<const ActivitySummary> summary;
};


using Data = std::variant<
    UpdateState,

    Time,
    SettingsUpd,
    Battery,
    GpsFix,
    TrackStateUpd,
    TrackDataUpd,
    LapEnded,
    Summary
>;
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

    virtual void handleEvent(const S2GEvent::Time& event) = 0;
    virtual void handleEvent(const S2GEvent::SettingsUpd& event) = 0;
    virtual void handleEvent(const S2GEvent::Battery& event) = 0;
    virtual void handleEvent(const S2GEvent::GpsFix& event) = 0;
    virtual void handleEvent(const S2GEvent::TrackStateUpd& event) = 0;
    virtual void handleEvent(const S2GEvent::TrackDataUpd& event) = 0;
    virtual void handleEvent(const S2GEvent::LapEnded& event) = 0;
    virtual void handleEvent(const S2GEvent::Summary& event) = 0;


private:
    S2GEvent::State mGUIState;
};




namespace G2SEvent
{    // GUI-to-Service events
struct GuiRun {};
struct GuiStop {};

struct TrackStart {};
struct TrackStop {
    bool discard;   // If true, tarck will be discarded, otherwise saved
};
struct SettingsSave {
    Settings settings;
};


struct InternalRefresh {};

using Data = std::variant<
    GuiRun,
    GuiStop,
    TrackStart,
    TrackStop,
    SettingsSave,

    InternalRefresh
>;
};


class IServiceModelHandler {
public:
    virtual ~IServiceModelHandler() = default;

    virtual void handleEvent(const G2SEvent::GuiRun& event)  = 0;
    virtual void handleEvent(const G2SEvent::GuiStop& event) = 0;
    virtual void handleEvent(const G2SEvent::TrackStart& event) = 0;
    virtual void handleEvent(const G2SEvent::TrackStop& event) = 0;
    virtual void handleEvent(const G2SEvent::SettingsSave& event) = 0;

    virtual void handleEvent(const G2SEvent::InternalRefresh& event) = 0;
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
