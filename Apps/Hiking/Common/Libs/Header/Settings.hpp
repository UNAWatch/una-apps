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
    bool autoPauseEn;       ///< Flag to enable auto pause during activity track
    bool phoneNotifEn;      ///< Flag to enable receiving phone notification when app is run
    uint32_t alertSteps;    ///< Steps alert threshold for. 0 if not use.
    float alertDistance;    ///< Distance alert threshold in kilometers. 0 if not used.
    uint32_t alertTime;     ///< Activity time threshold. 0 if not use.
};


#endif /* __SETTINGS_SERIALIZER_HPP */