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


#include "Service/Software/Libs/Header/ActivityWriter.hpp"

#include <cassert>

#include "API/FileSystem.hpp"
#include "FileStreamBuf.hpp"

#include "fit_encode.hpp"
#include "fit_mesg_broadcaster.hpp"
#include "fit_file_id_mesg.hpp"
#include "fit_activity_mesg.hpp"
#include "fit_session_mesg.hpp"
#include "fit_lap_mesg.hpp"
#include "fit_record_mesg.hpp"
#include "fit_date_time.hpp"

#define LOG_MODULE_PRX      "ActivityWriter"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "UnaLogger/Logger.h"

ActivityWriter::ActivityWriter(const IKernel& kernel, const char* pathToDir) :
    mKernel(kernel), mPath(pathToDir)
{
    assert(pathToDir != nullptr);
}

bool ActivityWriter::createFile(const TrackData& track, const std::vector<LapData>& laps, const std::vector<PointData>& points)
{
    char buff[256] {};
    std::tm localTime {};
#if WIN32
    localtime_s(&localTime, &track.timeStart);
#else
    localtime_r(&track.timeStart, &localTime);
#endif

    // Create directory
    int len = snprintf(buff, sizeof(buff), "%s/%04u%02u/", mPath, localTime.tm_year + 1900, localTime.tm_mon + 1);
    if (len <= 0 || !mKernel.fs.mkdir(buff)) {
        LOG_ERROR("Failed to create dir [%s]\n", buff);
        return false;
    }

    // Create file 
    snprintf(&buff[len], sizeof(buff) - len, "activity_%04u%02u%02uT%02u%02u%02u.fit",
        localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday,
        localTime.tm_hour, localTime.tm_min, localTime.tm_sec);

    auto file = sdk::api::adapters::make_file_stream(&mKernel.fs, buff);

    if (!file || !file->open(true, true)) { // write mode, override
        LOG_ERROR("Failed to create file [%s]\n", buff);
    }

    fit::Encode encode { fit::ProtocolVersion::V20 };

    encode.Open(*file);

    fill(encode, track, laps, points);

    encode.Close();

    file->close();

    return true;
}

void ActivityWriter::fill(fit::Encode & encode, const TrackData& track, const std::vector<LapData>& laps, const std::vector<PointData>& points)
{

    // Developer Data ID Message
    fit::DeveloperDataIdMesg developerIdMesg {};

    // DeveloperId
    FIT_BYTE developerId[17] {};    // + '\0'
    snprintf((char*)developerId, 16, track.devID.c_str());
    for (int i = 0; i < 16; i++) {
        developerIdMesg.SetDeveloperId(i, developerId[i]);
    }

    // ApplicationId
    FIT_BYTE applicationId[17] {}; // + '\0'
    snprintf((char*)applicationId, 16, track.appID.c_str());
    for (int i = 0; i < 16; i++) {
        developerIdMesg.SetApplicationId(i, applicationId[i]);
    }

    developerIdMesg.SetManufacturerId(FIT_MANUFACTURER_DEVELOPMENT);
    developerIdMesg.SetDeveloperDataIndex(0);
    developerIdMesg.SetApplicationVersion(track.appVersion);
    encode.Write(developerIdMesg);


    // Field ID 0 - steps
    fit::FieldDescriptionMesg fieldDescRecord0;
    fieldDescRecord0.SetDeveloperDataIndex(0);
    fieldDescRecord0.SetFieldDefinitionNumber(0);
    fieldDescRecord0.SetFitBaseTypeId(FIT_BASE_TYPE_UINT32);
    fieldDescRecord0.SetFieldName(0, std::wstring(L"steps"));
    fieldDescRecord0.SetUnits(0, std::wstring(L"steps"));
    encode.Write(fieldDescRecord0);

    // Field ID 1 - floors
    fit::FieldDescriptionMesg fieldDescRecord1;
    fieldDescRecord1.SetDeveloperDataIndex(0);
    fieldDescRecord1.SetFieldDefinitionNumber(1);
    fieldDescRecord1.SetFitBaseTypeId(FIT_BASE_TYPE_UINT16);
    fieldDescRecord1.SetFieldName(0, std::wstring(L"floors"));
    fieldDescRecord1.SetUnits(0, std::wstring(L"floors"));
    encode.Write(fieldDescRecord1);



    // 1. File ID Message
    fit::FileIdMesg fileIdMesg;

    fileIdMesg.SetType(FIT_FILE_ACTIVITY);
    fileIdMesg.SetTimeCreated(unixToFitTimestamp(track.timeStart)); // Must be UTC time
    fileIdMesg.SetManufacturer(FIT_MANUFACTURER_DEVELOPMENT);
    fileIdMesg.SetProduct(0x554E); // "UN" в hex (U=0x55, N=0x4E)
    fileIdMesg.SetProductName(std::wstring(L"Hiking"));
    // TODO: add app ID

    encode.Write(fileIdMesg);


    // 2. Activity Message
    fit::ActivityMesg activityMesg;

    activityMesg.SetTimestamp(unixToFitTimestamp(track.timeStart)); // Must be UTC time

    std::tm localTime{};
#if WIN32
    localtime_s(&localTime, &track.timeStart);
#else
    localtime_r(&track.timeStart, &localTime);
#endif
    std::time_t timeStartLocal = tm2epoch(&localTime);

    activityMesg.SetLocalTimestamp(unixToFitTimestamp(timeStartLocal));// Must be local time

    encode.Write(activityMesg);


    // 3. Session Message
    fit::SessionMesg sessionMesg;

    sessionMesg.SetSport(FIT_SPORT_HIKING);
    sessionMesg.SetSubSport(FIT_SUB_SPORT_TRACK);

    sessionMesg.SetTimestamp(unixToFitTimestamp(track.timeStart));// Must be UTC time
    sessionMesg.SetStartTime(unixToFitTimestamp(track.timeStart));// Must be UTC time
    sessionMesg.SetTotalTimerTime(static_cast<FIT_FLOAT32>(track.duration)); // s
    sessionMesg.SetTotalElapsedTime(static_cast<FIT_FLOAT32>(track.duration)); // s

    sessionMesg.SetTotalDistance(track.totalDistance); // m
    sessionMesg.SetAvgSpeed(track.speedAvg); // m/s
    sessionMesg.SetMaxSpeed(track.speedMax); // m/s
    sessionMesg.SetAvgHeartRate(track.hrAvg);
    sessionMesg.SetMaxHeartRate(track.hrMax);
    if (track.elevation >= 0) {
        sessionMesg.SetTotalAscent(static_cast<FIT_UINT16>(track.elevation));
    } else {
        sessionMesg.SetTotalDescent(static_cast<FIT_UINT16>(track.elevation));
    }

    fit::DeveloperField stepsField(fieldDescRecord0, developerIdMesg);
    stepsField.AddValue(static_cast<FIT_UINT32>(track.steps));
    sessionMesg.AddDeveloperField(stepsField);

    fit::DeveloperField floorsField(fieldDescRecord1, developerIdMesg);
    floorsField.AddValue(static_cast<FIT_UINT16>(track.floors));
    sessionMesg.AddDeveloperField(floorsField);

    encode.Write(sessionMesg);


    // 4. Lap Messages
    for (const LapData& lap : laps) {
        fit::LapMesg lapMesg;

        lapMesg.SetSport(FIT_SPORT_HIKING);
        lapMesg.SetSubSport(FIT_SUB_SPORT_TRACK);

        lapMesg.SetTimestamp(unixToFitTimestamp(lap.timeStart));    // Must be UTC time
        lapMesg.SetStartTime(unixToFitTimestamp(lap.timeStart));    // Must be UTC time
        lapMesg.SetTotalTimerTime(static_cast<FIT_FLOAT32>(lap.duration)); // s
        lapMesg.SetTotalElapsedTime(static_cast<FIT_FLOAT32>(lap.elapsed)); // s

        lapMesg.SetTotalDistance(lap.distance); // m
        lapMesg.SetAvgSpeed(lap.speedAvg);  // m/s
        lapMesg.SetMaxSpeed(lap.speedMax);  // m/s
        lapMesg.SetAvgHeartRate(lap.hrAvg);
        lapMesg.SetMaxHeartRate(lap.hrMax);
        if (lap.elevation >= 0) {
            lapMesg.SetTotalAscent(static_cast<FIT_UINT16>(lap.elevation));
        } else {
            lapMesg.SetTotalDescent(static_cast<FIT_UINT16>(lap.elevation));
        }

        fit::DeveloperField stepsField(fieldDescRecord0, developerIdMesg);
        stepsField.AddValue(static_cast<FIT_UINT32>(lap.steps));
        lapMesg.AddDeveloperField(stepsField);

        fit::DeveloperField floorsField(fieldDescRecord1, developerIdMesg);
        floorsField.AddValue(static_cast<FIT_UINT16>(lap.floors));
        lapMesg.AddDeveloperField(floorsField);

        encode.Write(lapMesg);
    }

    // 5. Record Messages
    for (const PointData& point : points) {
        fit::RecordMesg recordMesg;

        recordMesg.SetTimestamp(unixToFitTimestamp(point.timestamp));   // Must be UTC time
        recordMesg.SetPositionLat(ConvertDegreesToSemicircles(point.latitude));
        recordMesg.SetPositionLong(ConvertDegreesToSemicircles(point.longitude));
        recordMesg.SetHeartRate(point.heartRate);
        
        fit::DeveloperField stepsField(fieldDescRecord0, developerIdMesg);
        stepsField.AddValue(static_cast<FIT_UINT32>(point.steps));
        recordMesg.AddDeveloperField(stepsField);

        fit::DeveloperField floorsField(fieldDescRecord1, developerIdMesg);
        floorsField.AddValue(static_cast<FIT_UINT16>(point.floors));
        recordMesg.AddDeveloperField(floorsField);

        encode.Write(recordMesg);
    }

}

std::time_t ActivityWriter::tm2epoch(const struct tm* tm)
{
    int y = tm->tm_year + 1900;
    int m = tm->tm_mon + 1;     // 1..12
    int d = tm->tm_mday;        // 1..31

    if (m <= 2) {
        y -= 1;
        m += 12;
    }

    // Julian day
    int64_t  era = (y >= 0 ? y : y - 399) / 400;
    uint32_t yoe = (uint32_t)(y - era * 400);
    uint32_t doy = (153 * (m - 3) + 2) / 5 + d - 1;
    uint32_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    int64_t days = era * 146097 + (int64_t)doe - 719468; // 1970-01-01 is 719468
    int64_t secs = days * 86400 + tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;

    return (time_t)secs;
}

FIT_DATE_TIME ActivityWriter::unixToFitTimestamp(std::time_t unixTimestamp)
{
    const std::time_t FIT_EPOCH_OFFSET = 631065600;
    return static_cast<FIT_DATE_TIME>(unixTimestamp - FIT_EPOCH_OFFSET);
}

// Convert degrees to semicircles
FIT_SINT32 ActivityWriter::ConvertDegreesToSemicircles(float degrees)
{
    return (FIT_SINT32)(degrees * (2147483648.0 / 180.0));
}