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

#include "JsonStreamWriter.hpp"
#include "JsonStreamReader.hpp"

#include "IKernel.hpp"

ActivitySummarySerializer::ActivitySummarySerializer(Interface::IFileSystem &fs,
        const char *pathToFile) :
        mFs(fs), mPath(pathToFile)
{
    assert(pathToFile != nullptr);
}

bool ActivitySummarySerializer::save(const ActivitySummary &summary)
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
    if (!summary.file.empty()) {
        writer.add("file", summary.file.data());
    }

    char timeStr[32] { };
    kernel->time.strftime(timeStr, sizeof(timeStr), "%Y%m%dT%H%M%S",
            summary.date);
    writer.add("date", timeStr);

    writer.add("time", summary.time);
    writer.add("distance", summary.distance);
    writer.add("speed_avg", summary.speedAvg);
    writer.add("steps", summary.steps);
    writer.add("elevation", summary.elevation);
    writer.add("pace_avg", summary.paceAvg);
    writer.add("hr_max", summary.hrMax);
    writer.add("bool", true);
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

    char* buffer = static_cast<char*>(kernel->mem.malloc(fileSize));
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

    // If any fields are missing, just ignore it.

    // Reading the 'file' field
    const char *pFile = nullptr;
    size_t fileLen = 0;
    if (reader.get("file", pFile, fileLen)) {
        summary.file = std::move(std::string(pFile, fileLen));
    }

    kernel->app.log("time start\n");
    // Reading the 'date' field
    const char *pDate = nullptr;
    size_t dateLen = 0;
    if (reader.get("date", pDate, dateLen)) {
        int year = 0;
        int month = 0;
        int day = 0;
        int hour = 0;
        int minute = 0;
        int second = 0;

        // Expected format: %Y%m%dT%H%M%S%
        if (sscanf(pDate, "%4d%2d%2dT%2d%2d%2d", 
            &year, &month, &day, &hour,  &minute, &second) == 6) 
        {
            summary.date.tm_year = year - 1900;
            summary.date.tm_mon = month - 1;
            summary.date.tm_mday = day;
            summary.date.tm_hour = hour;
            summary.date.tm_min = minute;
            summary.date.tm_sec = second;

            // Apply DST from current time
            summary.date.tm_isdst = kernel->time.dstOffset() > 0 ? 1 : 0;
        }
    }
    kernel->app.log("time end\n");

    // Reading numeric fields
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


    kernel->mem.free(buffer);

    return true;
}
