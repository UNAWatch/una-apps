/**
 ******************************************************************************
 * @file    S2GEvents.hpp
 * @date    06-10-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Defines user event types exchanged between the Service and GUI layers.
 *
 * @details
 * This header describes user-defined event data types that are used for
 * inter-thread communication between the Service and GUI parts of the
 * application.
 *
 * It **must** be included in every user application, even if no custom
 * event types are defined.
 * At least one event type � **std::monostate** � must always be present.
 *
 * The 'std::monostate' event is used as a "dummy" signal, for example,
 * to abort waiting for a queue pop operation or to signal a timeout.
 *
 * @note
 * - The user may freely extend the 'S2GEvent::Data' variant with additional
 *   structures representing their own events.
 * - For each new event type, a corresponding 'handleEvent()' method must
 *   be declared in 'IGUIModelHandler'.
 *
 ******************************************************************************
 * @attention
 * Do not remove or rename this file � it is a required part of the framework.
 * Even an empty application must include it, as it defines base event behavior.
 ******************************************************************************
 */

/**
 * @defgroup S2GEvents Service-to-GUI Event Types
 * @brief User-defined event data types sent from Service to GUI.
 * @{
 */

#pragma once

#include <cstdint>
#include <variant>
#include <string>
#include <memory>
#include <array>

// Application types
#include "Settings.hpp"
#include "TrackInfo.hpp"
#include "ActivitySummary.hpp"


/**
 * @namespace S2GEvent
 * @brief Namespace that groups all event data types used for communication
 *        from the Service layer to the GUI.
 */
namespace S2GEvent
{

/**
 * @brief Default "empty" event type.
 *
 * This type is always required. It is typically used to:
 * - Interrupt waiting on a queue.
 * - Represent "no data" or a neutral signal.
 */
using Default = std::monostate;

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

/**
 * @brief Define all possible event types transmitted from Service to GUI.
 *
 * @note The user may extend this list by adding their own types below.
 *       Each new type must also have a corresponding handler in
 *       'IGUIModelHandler'.
 */
using Data = std::variant<
    Default

    // User-defined event data types
    , Time
    , SettingsUpd
    , Battery
    , GpsFix
    , TrackStateUpd
    , TrackDataUpd
    , LapEnded
    , Summary

>;
}; // namespace S2GEvent


/**
 * @brief Interface for handling GUI model events.
 *
 * Each event type in 'S2GEvent::Data' must have a matching handler here.
 */
class IGUIModelHandler {
public:
    virtual ~IGUIModelHandler() = default;

    /**
     * @brief Default handler for std::monostate events.
     *
     * Usually used to handle "abort" or "no-op" signals.
     * The user may override this method if necessary.
     */
    virtual void handleEvent(const std::monostate& event) {};

    // User-defined handlers
    virtual void handleEvent(const S2GEvent::Time& event) = 0;
    virtual void handleEvent(const S2GEvent::SettingsUpd& event) = 0;
    virtual void handleEvent(const S2GEvent::Battery& event) = 0;
    virtual void handleEvent(const S2GEvent::GpsFix& event) = 0;
    virtual void handleEvent(const S2GEvent::TrackStateUpd& event) = 0;
    virtual void handleEvent(const S2GEvent::TrackDataUpd& event) = 0;
    virtual void handleEvent(const S2GEvent::LapEnded& event) = 0;
    virtual void handleEvent(const S2GEvent::Summary& event) = 0;

};

/** @} */ // end of S2GEvents
