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


/**
 * @namespace S2GEvent
 * @brief Namespace that groups all event data types used for communication
 *        from the Service layer to the GUI.
 */
namespace S2GEvent {

/**
 * @brief Default "empty" event type.
 *
 * This type is always required. It is typically used to:
 * - Interrupt waiting on a queue.
 * - Represent "no data" or a neutral signal.
 */
using Default = std::monostate;

// Heart rate info
    struct HeartRate {
        float heartRate;
        float trustLevel;
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
        , HeartRate

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
    virtual void handleEvent(const S2GEvent::HeartRate& event) = 0;

};

/** @} */ // end of S2GEvents
