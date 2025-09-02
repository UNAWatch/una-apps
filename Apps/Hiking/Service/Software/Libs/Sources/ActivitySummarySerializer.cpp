/**
 ******************************************************************************
 * @file    ActivitySummarySerializer.cpp
 * @date    08-04-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Serializes/Deserializes summary data to a file.
 ******************************************************************************
 *
 ******************************************************************************
 */

#include "ActivitySummarySerializer.hpp"

#include <cassert>

#include "SDK/JsonStreamWriter.hpp"
#include "SDK/JsonStreamReader.hpp"

#define LOG_MODULE_PRX      "ActivitySummarySerializer::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

ActivitySummarySerializer::ActivitySummarySerializer(const IKernel& kernel,
        const char *pathToFile) :
    mKernel(kernel), mPath(pathToFile)
{
    assert(pathToFile != nullptr);
}

bool ActivitySummarySerializer::save(const ActivitySummary &summary)
{
    const char *slash = strrchr(mPath, '/');
    if (slash) {
        char buff[sdk::api::FileSystem::skMaxPathLen]{ };
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

    writer.add("utc", static_cast<uint32_t>(summary.utc));
    writer.add("time", static_cast<uint32_t>(summary.time));
    writer.add("distance", summary.distance);
    writer.add("speed_avg", summary.speedAvg);
    writer.add("steps", summary.steps);
    writer.add("elevation", summary.elevation);
    writer.add("pace_avg", summary.paceAvg);
    writer.add("hr_max", summary.hrMax);
    writer.add("hr_avg", summary.hrAvg);
    
    const uint8_t *points = reinterpret_cast<const uint8_t*>(summary.map.points.data());
    writer.addHexString("map", points, summary.map.points.size() * 2);

    writer.endMap();

    file->flush();
    file->close();

    return true;
}

bool ActivitySummarySerializer::load(ActivitySummary &summary)
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

    // If any fields are missing, just ignore it.

    reader.get("utc", summary.utc);
    reader.get("time", summary.time);
    reader.get("distance", summary.distance);
    reader.get("speed_avg", summary.speedAvg);
    reader.get("steps", summary.steps);
    reader.get("elevation", summary.elevation);
    reader.get("pace_avg", summary.paceAvg);
    reader.get("hr_max", summary.hrMax);
    reader.get("hr_avg", summary.hrAvg);


#if 1
    // Track map as HEX-String
    const char *hexStr = nullptr;
    size_t hexStrLen = 0;

    if (reader.get("map", hexStr, hexStrLen) && hexStrLen % 4 == 0) {
        summary.map.points.reserve(hexStrLen / 4);
        for (size_t i = 0; i < hexStrLen / 4; i++) {
            TrackMapScreen::Point point { };
            char xstr[3] = { hexStr[i * 4], hexStr[i * 4 + 1], 0 };
            char ystr[3] = { hexStr[i * 4 + 2], hexStr[i * 4 + 3], 0 };
            point.x = static_cast<uint8_t>(strtol(xstr, nullptr, 16));
            point.y = static_cast<uint8_t>(strtol(ystr, nullptr, 16));
            summary.map.points.push_back(point);
        }
    }
#endif


    delete[] buffer;

    return true;
}
