/**
 ******************************************************************************
 * @file    SettingsSerializer.cpp
 * @date    08-04-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Serializes/Deserializes settings to a file.
 ******************************************************************************
 *
 ******************************************************************************
 */
#include "Service/Software/Libs/Header/SettingsSerializer.hpp"

#include <cassert>

#include "JsonStreamWriter.hpp"
#include "JsonStreamReader.hpp"


#define LOG_MODULE_PRX      "SettingsSerializer"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "UnaLogger/Logger.h"


SettingsSerializer::SettingsSerializer(const IKernel& kernel,
    const char *pathToFile) :
    mKernel(kernel), mPath(pathToFile)
{
    assert(pathToFile != nullptr);
}

bool SettingsSerializer::save(const Settings &settings)
{
    const char *slash = strrchr(mPath, '/');
    if (slash) {
        char buff[sdk::api::FileSystem::skMaxPathLen] { };
        snprintf(buff, sizeof(buff), "%.*s", static_cast<size_t>(slash - mPath), mPath);
        // Create dir
        if (!mKernel.fs.mkdir(buff)) {
            return false;
        }
    }

    std::unique_ptr<sdk::api::File> file = mKernel.fs.file(mPath);

    if (!file) {
        return false;
    }

    if (!file->open(true, true)) {
        file.reset();
        return false;
    }

    sdk::JsonStreamWriter writer(file.get());

    writer.startMap();

    writer.add("auto_pause_en", settings.autoPauseEn);
    writer.add("phone_notif_en", settings.phoneNotifEn);
    writer.add("alert_steps", settings.alertSteps);
    writer.add("alert_distance", settings.alertDistance);
    writer.add("alert_time", settings.alertTime);

    writer.endMap();

    file->flush();
    file->close();

    return true;
}

bool SettingsSerializer::load(Settings &settings)
{
    std::unique_ptr<sdk::api::File> file = mKernel.fs.file(mPath);

    if (!file) {
        return false;
    }

    if (!file->exist()) {
        file.reset();
        return false;
    }

    if (!file->open(false, false)) {
        file.reset();
        return false;
    }

    size_t fileSize = file->size();
    if (fileSize == 0) { // Check for adequate size
        file->close();
        file.reset();
        return false;
    }

    char* buffer = new (std::nothrow)char[fileSize];
    if (buffer == nullptr) {
        file->close();
        file.reset();
        return false;
    }

    size_t read = 0;
    bool status = (file->read(buffer, fileSize, read) || read != fileSize);

    file->close();
    file.reset();

    if (!status) {
        delete[] buffer;
        return false;
    }

    sdk::JsonStreamReader reader(buffer, fileSize);

    if (!reader.validate()) {
        LOG_ERROR("JSON is invalid\n");
        delete[] buffer;
        return false;
    }

    reader.get("auto_pause_en", settings.autoPauseEn);
    reader.get("phone_notif_en", settings.phoneNotifEn);
    reader.get("alert_steps", settings.alertSteps);
    reader.get("alert_distance", settings.alertDistance);
    reader.get("alert_time", settings.alertTime);

    delete[] buffer;

    return true;
}
