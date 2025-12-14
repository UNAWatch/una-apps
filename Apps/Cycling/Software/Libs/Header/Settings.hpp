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

 /**
  * @struct Settings
  * @brief Represents a settings of the app.
  */
struct Settings {
    bool autoPauseEn = false;   ///< Flag to enable auto pause during activity track
    bool phoneNotifEn = true;   ///< Flag to enable receiving phone notification when app is run
    float alertDistance = 0;    ///< Distance alert threshold in kilometers. 0 if not used.
    uint32_t alertTime = 0;     ///< Activity time threshold. 0 if not use.
    bool debugSkipGpsFix = false;    ///< Skip GPS fix to start activity
};


#endif /* __SETTINGS_HPP */
