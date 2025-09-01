/**
 ******************************************************************************
 * @file    Config.hpp
 * @date    19-01-2024
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Configuration for GUI.
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef __GUI_CONFIG_HPP
#define __GUI_CONFIG_HPP

#include <cstdint>
#include <cstdbool>

#include "touchgfx/Color.hpp"
#include <texts/TextKeysAndLanguages.hpp>

#define GUI_CONFIG_MS_2_TICKS(ms) ((ms)/(1000 / Gui::Config::kFrameRate))
namespace Gui
{

namespace Config
{

constexpr uint32_t kFrameRate = 10;
constexpr uint32_t kMenuAnimationSteps = 4;
constexpr uint32_t kScreenTimeoutSteps = GUI_CONFIG_MS_2_TICKS(30 * 1000);          // 30s
constexpr uint32_t kConfirmTimeout = GUI_CONFIG_MS_2_TICKS(1 * 1000);               // 1s
constexpr uint32_t kTrackLapScreenTimeout = GUI_CONFIG_MS_2_TICKS(5 * 1000);        // 5s
constexpr uint32_t kTrackTitleInfoSwitchPeriod = GUI_CONFIG_MS_2_TICKS(3 * 1000);   // 3s
constexpr uint32_t kTrackActionConfirmScreenTimeout = GUI_CONFIG_MS_2_TICKS(2 * 1000);  // 2s

namespace Button
{
constexpr uint8_t L1 = '1';
constexpr uint8_t L2 = '2';
constexpr uint8_t R1 = '3';
constexpr uint8_t R2 = '4';
constexpr uint8_t L1R2 = 'z';
} // Button

namespace Battery
{
// Battery Thresholds 0 - Lo - Hi - 100
constexpr uint8_t kThresholdHi = 85;
constexpr uint8_t kThresholdLo = 25;
constexpr uint8_t kLowBatteryThreshold = 5;
}; // Battery


namespace HeartRate // TODO: get from settings
{
// HR bar Thresholds %: gray <= th1 < green <= th2 < yellow <= th3 < orange <= th4 < red
constexpr uint8_t kThreshold1 = 80;
constexpr uint8_t kThreshold2 = 90;
constexpr uint8_t kThreshold3 = 100;
constexpr uint8_t kThreshold4 = 110;
}; // HeartRate


namespace Color
{
inline const touchgfx::colortype WHITE = 0xC0C0C0;
inline const touchgfx::colortype GRAY = 0x808080;
inline const touchgfx::colortype GRAY_DARK = 0x404040;
inline const touchgfx::colortype BLACK = 0x000000;
inline const touchgfx::colortype YELLOW_DARK = 0xC08000;
inline const touchgfx::colortype TEAL = 0x008080;
inline const touchgfx::colortype TEAL_DARK = 0x004040;
inline const touchgfx::colortype GREEN = 0x00C000;
inline const touchgfx::colortype DARK_GREEN = 0x004000;
inline const touchgfx::colortype BROUN = 0xC08040;
inline const touchgfx::colortype RED = 0xC00000;
inline const touchgfx::colortype DARK_RED = 0x400000;
inline const touchgfx::colortype BLUE = 0x0000C0;
inline const touchgfx::colortype DARK_BLUE = 0x000040;
inline const touchgfx::colortype CYAN = 0x40C0C0;
inline const touchgfx::colortype CYAN_LIGHT = 0x80C0C0;

inline const touchgfx::colortype MENU_NOT_SELECTED_TIP_ON = YELLOW_DARK;
inline const touchgfx::colortype MENU_NOT_SELECTED_TIP_OFF = TEAL;

} // namespace Color

} // namespace Config

namespace Menu
{
enum Id {
    ID_START = 0,
    ID_SETTINGS,
    ID_LAST_ACTIVITY,

    ID_COUNT,               // Total number of menu items
    ID_DEFAULT = ID_START   // Default selected item
};
} // Menu

namespace Menu::Start::Track {
    enum Id {
        ID_TRACK1 = 0,
        ID_TRACK2,
        ID_TRACK3,
        ID_TRACK4,

        ID_COUNT,               // Total number of menu items
        ID_DEFAULT = ID_START   // Default selected item
    };
} // Menu::Start::Track

namespace Menu::Start::Action
{
enum Id {
    ID_RESUME = 0,
    ID_SAVE,
    ID_DISCARD,

    ID_COUNT,               // Total number of menu items
    ID_DEFAULT = ID_RESUME  // Default selected item
};
} // Menu::Start::Action

namespace Menu::Settings
{
enum Id {
    ID_ALERTS = 0,
    ID_AUTO_PAUSE,
    ID_PHONE_NOTIF,

    ID_COUNT,               // Total number of menu items
    ID_DEFAULT = ID_ALERTS  // Default selected item
};
} // Menu::Settings

namespace Menu::Settings::Alerts
{
enum Id {
    ID_STEPS = 0,
    ID_DISTANCE,
    ID_TIME,

    ID_COUNT,               // Total number of menu items
    ID_DEFAULT = ID_STEPS   // Default selected item
};
} // Menu::Settings::Alerts

namespace Menu::Settings::Alerts::Steps
{
enum Id {
    ID_OFF = 0,
    ID_STEPS_500,
    ID_STEPS_1000,
    ID_STEPS_2000,
    ID_STEPS_5000,
    ID_STEPS_10000,

    ID_COUNT,               // Total number of menu items
    ID_DEFAULT = ID_OFF     // Default selected item
};
} //Menu::Settings::Alerts::Steps

namespace Menu::Settings::Alerts::Distance
{
enum Id {
    ID_OFF = 0,
    ID_KM_MILL_1,
    ID_KM_MILL_2,
    ID_KM_MILL_3,
    ID_KM_MILL_4,
    ID_KM_MILL_5,
    ID_KM_MILL_10,

    ID_COUNT,               // Total number of menu items
    ID_DEFAULT = ID_OFF     // Default selected item
};
} // Menu::Settings::Alerts::Distance

namespace Menu::Settings::Alerts::Time
{
enum Id {
    ID_OFF = 0,
    ID_MIN_10,
    ID_MIN_20,
    ID_MIN_30,
    ID_MIN_60,

    ID_COUNT,               // Total number of menu items
    ID_DEFAULT = ID_OFF     // Default selected item
};
} // Menu::Settings::Alerts::Time

static constexpr uint16_t kStepsList[Gui::Menu::Settings::Alerts::Steps::ID_COUNT] = { 0,500,1000,2000,5000,10000 };
static constexpr uint16_t kDistanceList[Gui::Menu::Settings::Alerts::Distance::ID_COUNT] = { 0,1,2,3,4,5,10 };
static constexpr uint16_t kTimeList[Gui::Menu::Settings::Alerts::Time::ID_COUNT] = { 0,10,20,30,60 };

struct TrackInfo {

    // Pace in seconds/km
    uint32_t pace;
    uint32_t avgPace;
    uint32_t lapPace;

    // Distance in km
    float totalDistance;
    float lapDistance;

    // Time in seconds
    time_t totalTime;
    time_t lapTime;

    uint32_t lapNum;

    float HR;
    float avgHR;
    float maxHR;

    // Speed in km/h
    float speed;
    float avgSpeed;
    float lapSpeed;

    float elevation;

    uint32_t steps;
    uint32_t lapSteps;

    uint32_t floors;
};

namespace Utils
{

/**
 * @brief   Convert kilometers to miles.
 * @param   km: Value in kilometers.
 * @return  Value in miles.
 */
inline float km2mi(float km)
{
    return km * 0.621371f;
}

/**
 * @brief   Convert miles to kilometers.
 * @param   mi: Value in miles.
 * @return  Value in kilometers.
 */
inline float mi2km(float mi)
{
    return mi / 0.621371f;
}

/**
 * @brief   Convert meters to feet.
 * @param   m: Value in meters.
 * @return  Value in feet.
 */
inline float m2ft(float m)
{
    return m * 3.28084f;
}

/**
 * @brief   Convert feet to meters.
 * @param   ft: Value in feet.
 * @return  Value in meters.
 */
inline float ft2m(float ft)
{
    return ft / 3.28084f;
}

/**
* @brief   Round a target value to the nearest value in an array.
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

/**
 * @brief   Convert seconds to hours, minutes and seconds.
 * @param   sec: Seconds to convert.
 * @param   h: Reference to store hours.
 * @param   m: Reference to store minutes.
 * @param   s: Reference to store seconds.
 */
inline void sec2hms(uint32_t sec, uint16_t& h, uint8_t& m, uint8_t& s)
{
    h = static_cast<uint16_t>(sec / 3600);
    sec %= 3600;
    m = static_cast<uint8_t>(sec / 60);
    s = static_cast<uint8_t>(sec % 60);
}

/**
 * @brief   Convert seconds to hours.
 * @param   sec: Seconds to convert.
 * @return  Hours.
 */
inline uint16_t sec2hmsH(uint32_t sec)
{
    uint16_t h;
    uint8_t m;
    uint8_t s;
    sec2hms(sec, h, m, s);
    return h;
}

/**
 * @brief   Convert seconds to minutes.
 * @param   sec: Seconds to convert.
 * @return  Minutes.
 */
inline uint8_t sec2hmsM(uint32_t sec)
{
    uint16_t h;
    uint8_t m;
    uint8_t s;
    sec2hms(sec, h, m, s);
    return m;
}

/**
 * @brief   Convert seconds to seconds.
 * @param   sec: Seconds to convert.
 * @return  Seconds.
 */
inline uint8_t sec2hmsS(uint32_t sec)
{
    uint16_t h;
    uint8_t m;
    uint8_t s;
    sec2hms(sec, h, m, s);
    return s;
}

} // namespace Utils

} // namespace Gui

#endif /* __GUI_CONFIG_HPP */
