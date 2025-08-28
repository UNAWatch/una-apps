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
#include "SettingsSerializer.hpp"

#include <cassert>

#include "JsonStreamWriter.hpp"
#include "JsonStreamReader.hpp"

#include "IKernel.hpp"

#include "gui/common/GuiCommon.hpp"

SettingsSerializer::SettingsSerializer(Interface::IFileSystem &fs,
    const char *pathToFile) :
    mFs(fs), mPath(pathToFile)
{
    assert(pathToFile != nullptr);
}

bool SettingsSerializer::save(const Settings &settings)
{
    const char *slash = strrchr(mPath, '/');
    if (slash) {
        char buff[Interface::IFileSystem::skMaxPathLen] { };
        snprintf(buff, sizeof(buff), "%.*s", static_cast<size_t>(slash - mPath),
            mPath);
        // Create dir
        if (!mFs.mkdir(buff)) {
            return false;
        }
    }

    std::unique_ptr<Interface::IFile> file = mFs.file(mPath);

    if (!file) {
        return false;
    }

    if (!file->open(true, true)) {
        file.reset();
        return false;
    }

    JsonStreamWriter writer(file.get());

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
    std::unique_ptr<Interface::IFile> file = mFs.file(mPath);

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

    char *buffer = static_cast<char *>(kernel->mem.malloc(fileSize));
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
        kernel->mem.free(buffer);
        return false;
    }

    JsonStreamReader reader(buffer, fileSize);

    if (!reader.validate()) {
        kernel->app.log("JSON is invalid\n");
        kernel->mem.free(buffer);
        return false;
    }

    reader.get("auto_pause_en", settings.autoPauseEn);
    reader.get("phone_notif_en", settings.phoneNotifEn);

    uint32_t steps = 0;
    if (reader.get("alert_steps", steps) &&
        steps >= Gui::kStepsList[0] &&
        steps <= Gui::kStepsList[Gui::Menu::Settings::Alerts::Steps::MAX - 1]) 
    {
        settings.alertSteps = steps;
    }

    float distance = 0;
    if (reader.get("alert_distance", distance) &&
        distance >= Gui::kDistanceList[0] &&
        distance <= Gui::kDistanceList[Gui::Menu::Settings::Alerts::Distance::ID_COUNT - 1]) 
    {
        settings.alertDistance = distance;
    }

    uint32_t time = 0;
    if (reader.get("alert_time", time) &&
        time >= Gui::kTimeList[0] &&
        time <= Gui::kTimeList[Gui::Menu::Settings::Alerts::Time::MAX - 1]) {
        settings.alertTime = time;
    }

    kernel->mem.free(buffer);

    return true;
}
