/**
 ******************************************************************************
 * @file    SettingsSerializer.hpp
 * @date    08-04-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Serializes/Deserializes settings to a file.
 ******************************************************************************
 *
 * The serialized format follows this structure:
 *
 * @code{.json}
 * {
 *     "auto_pause_en":false,
 *     "phone_notif_en":false,
 *     "alert_steps":0,
 *     "alert_distance":0.0,
 *     "alert_time":0
 * }
 * @endcode
 *
 ******************************************************************************
 */

#ifndef __SETTINGS_SERIALIZER_HPP
#define __SETTINGS_SERIALIZER_HPP

#include <cstdint>
#include <cstdbool>
#include <string>

#include "IKernel.hpp"

#include "Common/Header/Settings.hpp"


 /**
  * @class SettingsSerializer
  * @brief Serializes/Deserializes settings to a file.
  */
class SettingsSerializer {

public:

    /**
     * @brief Constructor.
     * @param kernel: Reference to the kernel interface.
     * @param pathToFile: Path of the settings file.
     */
    SettingsSerializer(const IKernel& kernel, const char *pathToFile);

    /**
     * @brief Destructor.
     */
    virtual ~SettingsSerializer() = default;

    /**
     * @brief Save settings.
     * @param summary: Reference to the Settings structure.
     * @return True if settings saved successfully.
     */
    bool save(const Settings &settings);

    /**
     * @brief Save settings.
     * @param summary: Reference to load the Settings structure.
     * @return True if settings read successfully.
     */
    bool load(Settings &settings);

private:
    /// A constant reference to an IKernel object.
    const IKernel& mKernel;

    /// Path to settings file
    const char *mPath = nullptr;    
};

#endif /* __SETTINGS_SERIALIZER_HPP */
