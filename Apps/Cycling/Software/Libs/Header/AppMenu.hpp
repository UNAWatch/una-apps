/**
 ******************************************************************************
 * @file    AppMenu.hpp
 * @date    25-09-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Menu configuration.
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef __APP_MENU_HPP
#define __APP_MENU_HPP

#include <cstdint>
#include <cmath>

namespace App::Menu
{
enum Id {
    ID_START = 0,
    ID_SETTINGS,
    ID_LAST_ACTIVITY,

    ID_COUNT,               // Total number of menu items
    ID_DEFAULT = ID_START   // Default selected item
};


namespace Start::Track
{
enum Id {
    ID_TRACK1 = 0,
    ID_TRACK2,
    ID_TRACK3,
    ID_TRACK4,

    ID_COUNT,               // Total number of menu items
    ID_DEFAULT = ID_START   // Default selected item
};
} // Start::Track

namespace Start::Action
{
enum Id {
    ID_RESUME = 0,
    ID_SAVE,
    ID_DISCARD,

    ID_COUNT,               // Total number of menu items
    ID_DEFAULT = ID_RESUME  // Default selected item
};
} // Start::Action

namespace Settings
{
enum Id {
    ID_ALERTS = 0,
    ID_AUTO_PAUSE,
    ID_PHONE_NOTIF,

    ID_COUNT,               // Total number of menu items
    ID_DEFAULT = ID_ALERTS  // Default selected item
};
} // Settings

namespace Settings::Alerts
{
enum Id {
    ID_DISTANCE = 0,
    ID_TIME,

    ID_COUNT,                   // Total number of menu items
    ID_DEFAULT = ID_DISTANCE    // Default selected item
};
} // Settings::Alerts

namespace Settings::Alerts::Distance
{
enum Id {
    ID_OFF = 0,
    ID_KM_MILL_1,
    ID_KM_MILL_5,
    ID_KM_MILL_10,
    ID_KM_MILL_15,
    ID_KM_MILL_20,
    ID_KM_MILL_25,

    ID_COUNT,                   // Total number of menu items
    ID_DEFAULT = ID_KM_MILL_1   // Default selected item
};
} // Settings::Alerts::Distance

namespace Settings::Alerts::Time
{
enum Id {
    ID_OFF = 0,
    ID_MIN_10,
    ID_MIN_20,
    ID_MIN_30,
    ID_MIN_60,
    ID_MIN_120,

    ID_COUNT,               // Total number of menu items
    ID_DEFAULT = ID_OFF     // Default selected item
};
} // Settings::Alerts::Time

static constexpr uint16_t kDistanceList[Settings::Alerts::Distance::ID_COUNT] = { 0,1,5,10,15,20,25 };
static constexpr uint16_t kTimeList[Settings::Alerts::Time::ID_COUNT] = { 0,10,20,30,60,120 };


/**
* @brief   Round a target value to the nearest value in an array.
*
* Helper function for snapping a metric to the closest value from the menu.
*
* @param   arr: Pointer to the array of values.
* @param   len: Length of the array.
* @param   target: Target value to round.
* @retval  Nearest value in the array to the target.
*/
template <typename T>
inline T RoundToNearest(T* arr, size_t len, float target)
{
    T nearest = arr[0];
    float minDiff = std::abs(static_cast<float>(arr[0]) - target);

    for (size_t i = 1; i < len; ++i) {
        float diff = std::abs(static_cast<float>(arr[i]) - target);
        if (diff < minDiff) {
            minDiff = diff;
            nearest = arr[i];
        }
    }

    return nearest;
}

/**
 * @brief   Round a target value to the nearest value in an array and return its index.
 *
 * Helper function for binding a metric to the nearest index from the menu.
 *
 * @param   arr: Pointer to the array of values.
 * @param   len: Length of the array.
 * @param   target: Target value to round.
 * @retval  Index of the nearest value in the array to the target.
 */
template<typename T>
inline size_t RoundToNearestIndex(T* arr, size_t len, float target)
{
    size_t nearestIndex = 0;
    float minDiff = std::abs(static_cast<float>(arr[0]) - target);

    for (size_t i = 1; i < len; ++i) {
        float diff = std::abs(static_cast<float>(arr[i]) - target);
        if (diff < minDiff) {
            minDiff = diff;
            nearestIndex = i;
        }
    }

    return nearestIndex;
}

} // App::Menu

#endif /* __APP_MENU_HPP */
