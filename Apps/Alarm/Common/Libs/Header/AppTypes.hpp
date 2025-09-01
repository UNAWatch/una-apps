/**
 ******************************************************************************
 * @file    AppTypes.hpp
 * @date    24-07-2024
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Common application types and link interface between GUI and Service.
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef __APP_TYPES_HPP
#define __APP_TYPES_HPP

#include <cstdint>
#include <cstdbool>
#include <cstring>
#include <variant>
#include <memory>
#include <ctime>

/**
 * @namespace AppType
 * @brief Namespace for application-specific types.
 */
namespace AppType
{

/**
 * @struct Info
 * @brief Structure representing a single alarm's settings.
 */
struct Alarm {

/**
 * @enum Repeat
 * @brief Alarm repetition options.
 */
    enum Repeat : uint8_t {
        REPEAT_NO,
        REPEAT_EVERY_DAY,
        REPEAT_WEEK_DAYS,
        REPEAT_WEEKENDS,
        REPEAT_MONDAY,
        REPEAT_TUESDAY,
        REPEAT_WEDNESDAY,
        REPEAT_THURSDAY,
        REPEAT_FRIDAY,
        REPEAT_SATURDAY,
        REPEAT_SUNDAY,

        REPEAT_COUNT    /// Number of repeat options.
    };

    /**
     * @enum Effect
     * @brief Alarm effect options.
     */
    enum Effect : uint8_t {
        EFFECT_BEEP_AND_VIBRO,
        EFFECT_VIBRO,
        EFFECT_BEEP,

        EFFECT_COUNT    /// Number of effect options.
    };

    bool on;                ///< Alarm active state.
    uint8_t timeHours;      ///< Alarm hour.
    uint8_t timeMinutes;    ///< Alarm minute.
    Repeat repeat;          ///< Alarm repetition setting.
    Effect effect;          ///< Alarm effect setting.

    /**
     * @brief Equality operator to compare two alarm Info objects.
     * @param other: The other Info object to compare.
     * @return 'true' if the alarms are equal, 'false' otherwise.
     */
    bool operator==(const Alarm& other) const
    {
        return timeHours == other.timeHours && 
            timeMinutes == other.timeMinutes
            && repeat == other.repeat;
        // Ignore 'on' and 'effect' for equality check, as they do not define the alarm itself
    }

    /**
     * @brief Inequality operator to compare two alarm Info objects.
     * @param other: The other Info object to compare.
     * @return 'true' if the alarms are not equal, 'false' otherwise.
     */
    bool operator!=(const Alarm& other) const
    {
        return !(*this == other);
    }

    /**
     * @brief Assignment operator to copy the contents of another alarm Info object.
     * @param other: The other Info object to copy from.
     * @return Reference to the current object.
     */
    Alarm& operator=(const Alarm& other) {
        if (this != &other) {
            on = other.on;
            timeHours = other.timeHours;
            timeMinutes = other.timeMinutes;
            repeat = other.repeat;
            effect = other.effect;
        }
        return *this;
    }
};


} // namespace AppTypes

#endif // __APP_TYPES_HPP
