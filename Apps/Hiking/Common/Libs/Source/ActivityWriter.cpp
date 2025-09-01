/**
 ******************************************************************************
 * @file    ActivityWriter.hpp
 * @date    08-04-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Serializes activity data to a file.
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <cassert>
#include <cmath>

#include "ActivityWriter.hpp"

ActivityWriter::ActivityWriter(Interface::IFileSystem &fs,
        const char *pathToDir) :
        mFs(fs), mBasePath(pathToDir)
{
}

bool ActivityWriter::startSession(const char *activityType, std::tm timestamp)
{

    if (!createFileAndOpen(timestamp)) {
        return false;
    }

    // Init stream writer
    mWriter.setOutput(mFile.get());
    mWriter.startMap(); // Start 'root' object
    mWriter.add("type", "running");

    char timeStr[32] { };
    snprintf(timeStr, sizeof(timeStr), "%04u%02u%02uT%02u%02u%02u",
            timestamp.tm_year + 1900, timestamp.tm_mon + 1, timestamp.tm_mday,
            timestamp.tm_hour, timestamp.tm_min, timestamp.tm_sec);
    mWriter.add("start_time", timeStr);

    mIsActive = true;
    return true;

}

bool ActivityWriter::endSession(uint16_t endTimeOffset)
{
    if (!mIsActive) {
        return false;
    }

    mWriter.add("end_time_offset", endTimeOffset);
    mWriter.endMap(); // End 'root' object

    mWriter.flush();

    closeFile();

    mFile.reset();

    return true;
}

bool ActivityWriter::deleteSession()
{
    if (!mFile) {
        return false;
    }

    if (!mFile->exist()) {
        return true;
    }

    if (mFile->isOpen()) {
        mFile->close();
    }

    mFile->remove();

    mFile.reset();
    return true;
}

const char* ActivityWriter::getFilePath() const
{
    if (!mFile) {
        return nullptr;
    }

    return mFile->getPath();
}

bool ActivityWriter::startLaps()
{
    if (!mIsActive) {
        return false;
    }
    mWriter.startArray("lap"); // Start 'lap' array
    return true;
}

bool ActivityWriter::endLaps()
{
    if (!mIsActive) {
        return false;
    }
    mWriter.endArray(); // End 'lap' array

    mWriter.flush();

    return true;
}

bool ActivityWriter::startLap(uint16_t startTimeOffset)
{
    if (!mIsActive) {
        return false;
    }

    mWriter.startMap(); // Start 'unnamed' lap object
    mWriter.add("st", startTimeOffset);
    mWriter.startArray("data");

    return true;
}

bool ActivityWriter::endLap(uint16_t endTimeOffset)
{
    if (!mIsActive) {
        return false;
    }

    mWriter.endArray(); // End 'data' array
    mWriter.add("et", endTimeOffset);
    mWriter.endMap();   // End 'unnamed' lap object

    mWriter.flush();

    return true;
}

bool ActivityWriter::addDataPoints(const DataPoint *pDataPoints, size_t count)
{
    if (!mIsActive || pDataPoints == nullptr) {
        return false;
    }

    for (size_t i = 0; i < count; i++) {
        const DataPoint &p = pDataPoints[i];

        CborStreamWriter::ArrayScope dataSample(mWriter);
        mWriter.add(p.timeOffset);
        mWriter.add(p.heartRate);
        mWriter.add(static_cast<int32_t>(p.location.latitude * 1000000));   // x1000000
        mWriter.add(static_cast<int32_t>(p.location.longitude * 1000000));  // x1000000
        mWriter.add(static_cast<int16_t>(p.location.altitude));
    }

    mWriter.flush();

    return true;
}


bool ActivityWriter::createFileAndOpen(std::tm timestamp)
{
    char buff[256] { };

    // Create directory
    int len = snprintf(buff, sizeof(buff), "%s%04u%02u/",
            mBasePath, timestamp.tm_year + 1900, timestamp.tm_mon + 1);
    if (len <= 0 || !mFs.mkdir(buff)) {
        return false;
    }

    // Create file 
    snprintf(&buff[len], sizeof(buff) - len,
            "activity_%04u%02u%02uT%02u%02u%02u.cbor",
            timestamp.tm_year + 1900, timestamp.tm_mon + 1, timestamp.tm_mday,
            timestamp.tm_hour, timestamp.tm_min, timestamp.tm_sec);

    mFile = mFs.file(buff);
    if (!mFile) {
        return false;
    }

    if (!mFile->open(true, true)) {
        mFile.reset();
        return false;
    }

    return true;
}

void ActivityWriter::closeFile()
{
    mFile->flush();
    mFile->close();
}
