/**
 ******************************************************************************
 * @file    Settings.hpp
 * @date    08-04-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Represents a settings of the app.
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef __SETTINGS_HPP
#define __SETTINGS_HPP

#include <cstdint>
#include <array>

 /**
  * @struct Settings
  * @brief Represents a settings of the app.
  */
struct Settings {
    bool autoPauseEn = false;   ///< Flag to enable auto pause during activity track
    bool phoneNotifEn = true;   ///< Flag to enable receiving phone notification when app is run
    uint32_t alertSteps = 0;    ///< Steps alert threshold for. 0 if not use.
    float alertDistance = 0;    ///< Distance alert threshold in kilometers. 0 if not used.
    uint32_t alertTime = 0;     ///< Activity time threshold. 0 if not use.
};

/**
 * @brief   Heart rate thresholds array in beats per minute (BPM).
 */
inline constexpr std::array<uint8_t, 4> kHrThresholdsDefault = { 120, 140, 160, 170 };

#endif /* __SETTINGS_HPP */
