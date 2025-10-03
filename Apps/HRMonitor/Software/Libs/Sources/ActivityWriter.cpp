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


#include "ActivityWriter.hpp"

#include <cassert>
#include <cstring>

#include "SDK/Interfaces/IFileSystem.hpp"

extern "C" {
#include "fit_product.h"
#include "fit_crc.h"
}

#define LOG_MODULE_PRX      "ActivityWriter::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"




ActivityWriter::ActivityWriter(const IKernel& kernel, const char* pathToDir) :
    mKernel(kernel), mPath(pathToDir)
{
    assert(pathToDir != nullptr);
}

void ActivityWriter::start(const AppInfo& info)
{
    // Reset counter
    mLapCounter = 0;
    mDataCRC = 0;

    createAndOpenFile(info.timestamp);

    if (!mFile) {
        return;
    }

    SDK::Interface::IFile* fp = mFile.get();

    // Add empty header
    WriteFileHeader(fp);

    // Write file id message.
    {
        FIT_FILE_ID_MESG file_id_mesg{};
        Fit_InitMesg(fit_mesg_defs[FIT_MESG_FILE_ID], &file_id_mesg);

        file_id_mesg.serial_number = 0;
        file_id_mesg.time_created = unixToFitTimestamp(info.timestamp);
        strncpy(file_id_mesg.product_name, "UNA Watch", FIT_FILE_ID_MESG_PRODUCT_NAME_COUNT);
        file_id_mesg.manufacturer = FIT_MANUFACTURER_DEVELOPMENT;
        file_id_mesg.product = 0;
        file_id_mesg.number = 0;
        file_id_mesg.type = FIT_FILE_ACTIVITY;

        WriteMessageDefinition(skFileMsgNum, fit_mesg_defs[FIT_MESG_FILE_ID], FIT_FILE_ID_MESG_DEF_SIZE, fp);
        WriteMessage(skFileMsgNum, &file_id_mesg, FIT_FILE_ID_MESG_SIZE, fp);
    }

    // Developer Data ID Message 
    {
        FIT_DEVELOPER_DATA_ID_MESG developer_data_id_mesg{};
        Fit_InitMesg(fit_mesg_defs[FIT_MESG_DEVELOPER_DATA_ID], &developer_data_id_mesg);

        strncpy(reinterpret_cast<char*>(developer_data_id_mesg.developer_id), info.devID.c_str(), FIT_DEVELOPER_DATA_ID_MESG_DEVELOPER_ID_COUNT);
        strncpy(reinterpret_cast<char*>(developer_data_id_mesg.application_id), info.appID.c_str(), FIT_DEVELOPER_DATA_ID_MESG_APPLICATION_ID_COUNT);
        developer_data_id_mesg.application_version = info.appVersion;
        developer_data_id_mesg.manufacturer_id = FIT_MANUFACTURER_DEVELOPMENT;
        developer_data_id_mesg.developer_data_index = 0;

        WriteMessageDefinition(skDevelopMsgNum, fit_mesg_defs[FIT_MESG_DEVELOPER_DATA_ID], FIT_DEVELOPER_DATA_ID_MESG_DEF_SIZE, fp);
        WriteMessage(skDevelopMsgNum, &developer_data_id_mesg, FIT_DEVELOPER_DATA_ID_MESG_SIZE, fp);
    }

    WriteMessageDefinition(skLapMsgNum, fit_mesg_defs[FIT_MESG_LAP], FIT_LAP_MESG_DEF_SIZE, fp);
    WriteMessageDefinition(skSessionMsgNum, fit_mesg_defs[FIT_MESG_SESSION], FIT_SESSION_MESG_DEF_SIZE, fp);
    WriteMessageDefinition(skEventMsgNum, fit_mesg_defs[FIT_MESG_EVENT], FIT_EVENT_MESG_DEF_SIZE, fp);

    // Custom field "hr_trust_level"
    {
        FIT_FIELD_DESCRIPTION_MESG field_description_mesg_steps{};
        Fit_InitMesg(fit_mesg_defs[FIT_MESG_FIELD_DESCRIPTION], &field_description_mesg_steps);

        strncpy(field_description_mesg_steps.field_name, "hr_trust_level", FIT_FIELD_DESCRIPTION_MESG_FIELD_NAME_COUNT);
        strncpy(field_description_mesg_steps.units, "hr_trust_level", FIT_FIELD_DESCRIPTION_MESG_UNITS_COUNT);
        field_description_mesg_steps.developer_data_index = 0;
        field_description_mesg_steps.field_definition_number = 0;
        field_description_mesg_steps.fit_base_type_id = FIT_BASE_TYPE_UINT8;

        WriteMessageDefinition(skHrTrustLevelMsgNum, fit_mesg_defs[FIT_MESG_FIELD_DESCRIPTION], FIT_FIELD_DESCRIPTION_MESG_DEF_SIZE, fp);
        WriteMessage(skHrTrustLevelMsgNum, &field_description_mesg_steps, FIT_FIELD_DESCRIPTION_MESG_SIZE, fp);
    }

    // The message definition only needs to be written once.
    FIT_DEV_FIELD_DEF dev_field_def[1]{};
    // hr_trust_level
    dev_field_def[0].def_num = 0;
    dev_field_def[0].size = sizeof(FIT_UINT8);
    dev_field_def[0].dev_index = 0;
    WriteMessageDefinitionWithDevFields(skRecordMsgNum, fit_mesg_defs[FIT_MESG_RECORD], FIT_RECORD_MESG_DEF_SIZE, 1, dev_field_def, fp);

    // Write Event message - START Event
    {
        FIT_EVENT_MESG event_mesg{};
        Fit_InitMesg(fit_mesg_defs[FIT_MESG_EVENT], &event_mesg);

        event_mesg.timestamp = unixToFitTimestamp(info.timestamp);
        event_mesg.event = FIT_EVENT_TIMER;
        event_mesg.event_type = FIT_EVENT_TYPE_START;

        WriteMessage(skEventMsgNum, &event_mesg, FIT_EVENT_MESG_SIZE, fp);
    }

}

void ActivityWriter::pause()
{
    if (!mFile) {
        return;
    }
    SDK::Interface::IFile* fp = mFile.get();

    // Write Event message - STOP Event

    FIT_EVENT_MESG event_mesg{};
    Fit_InitMesg(fit_mesg_defs[FIT_MESG_EVENT], &event_mesg);

    event_mesg.timestamp = unixToFitTimestamp(std::time(nullptr));
    event_mesg.event = FIT_EVENT_TIMER;
    event_mesg.event_type = FIT_EVENT_TYPE_STOP;

    WriteMessage(skEventMsgNum, &event_mesg, FIT_EVENT_MESG_SIZE, fp);

}

void ActivityWriter::resume()
{
    if (!mFile) {
        return;
    }
    SDK::Interface::IFile* fp = mFile.get();

    // Write Event message - START Event

    FIT_EVENT_MESG event_mesg;
    Fit_InitMesg(fit_mesg_defs[FIT_MESG_EVENT], &event_mesg);

    event_mesg.timestamp = unixToFitTimestamp(std::time(nullptr));
    event_mesg.event = FIT_EVENT_TIMER;
    event_mesg.event_type = FIT_EVENT_TYPE_START;

    WriteMessage(skEventMsgNum, &event_mesg, FIT_EVENT_MESG_SIZE, fp);
    
}

void ActivityWriter::addRecord(const RecordData& record)
{
    if (!mFile) {
        return;
    }
    SDK::Interface::IFile* fp = mFile.get();

    FIT_RECORD_MESG record_mesg {};
    Fit_InitMesg(fit_mesg_defs[FIT_MESG_RECORD], &record_mesg);

    record_mesg.timestamp = unixToFitTimestamp(record.timestamp);
    record_mesg.heart_rate = record.heartRate;

    WriteMessage(skRecordMsgNum, &record_mesg, FIT_RECORD_MESG_SIZE, fp);

    FIT_UINT8 trustLevel = record.trustLevel;
    WriteDeveloperField(&trustLevel, sizeof(trustLevel), fp);
}

void ActivityWriter::addLap(const LapData& lap)
{
    if (!mFile) {
        return;
    }
    SDK::Interface::IFile* fp = mFile.get();

    FIT_LAP_MESG lap_mesg {};
    Fit_InitMesg(fit_mesg_defs[FIT_MESG_LAP], &lap_mesg);

    lap_mesg.message_index = 0;

    lap_mesg.timestamp = unixToFitTimestamp(lap.timestamp);
    lap_mesg.start_time = unixToFitTimestamp(lap.timeStart);
    lap_mesg.total_elapsed_time = static_cast<FIT_UINT32>((lap.elapsed) * 1000);
    lap_mesg.total_timer_time = static_cast<FIT_UINT32>((lap.duration) * 1000);

    lap_mesg.avg_heart_rate = static_cast<FIT_UINT8>(lap.hrAvg);// 1 * bpm + 0, average heart rate (excludes pause time)
    lap_mesg.max_heart_rate = static_cast<FIT_UINT8>(lap.hrMax); // 1 * bpm + 0,

    WriteMessage(skLapMsgNum, &lap_mesg, FIT_LAP_MESG_SIZE, fp);

    mLapCounter++;
}

void ActivityWriter::stop(const TrackData& track)
{
    if (!mFile) {
        return;
    }

    SDK::Interface::IFile* fp = mFile.get();

    // Write Event message - STOP Event
    {
        FIT_EVENT_MESG event_mesg {};
        Fit_InitMesg(fit_mesg_defs[FIT_MESG_EVENT], &event_mesg);

        event_mesg.timestamp = unixToFitTimestamp(std::time(nullptr));
        event_mesg.event = FIT_EVENT_TIMER;
        event_mesg.event_type = FIT_EVENT_TYPE_STOP;

        WriteMessage(skEventMsgNum, &event_mesg, FIT_EVENT_MESG_SIZE, fp);
    }

    // Write Session message.
    {
        FIT_SESSION_MESG session_mesg {};
        Fit_InitMesg(fit_mesg_defs[FIT_MESG_SESSION], &session_mesg);

        session_mesg.message_index = 0;
        session_mesg.sport = FIT_SPORT_GENERIC;
        session_mesg.sub_sport = FIT_SUB_SPORT_GENERIC; // TODO: select correct type
        session_mesg.timestamp = unixToFitTimestamp(track.timeStart);
        session_mesg.start_time = unixToFitTimestamp(track.timeStart);

        session_mesg.total_elapsed_time = static_cast<FIT_UINT32>((track.elapsed) * 1000); // 1000 * s + 0, Time (includes pauses)
        session_mesg.total_timer_time = static_cast<FIT_UINT32>((track.duration) * 1000);   // 1000 * s + 0, Timer Time (excludes pauses)


        session_mesg.avg_heart_rate = static_cast<FIT_UINT8>(track.hrAvg);// 1 * bpm + 0, average heart rate (excludes pause time)
        session_mesg.max_heart_rate = static_cast<FIT_UINT8>(track.hrMax); // 1 * bpm + 0,


        session_mesg.num_laps = 0;

        WriteMessage(skSessionMsgNum, &session_mesg, FIT_SESSION_MESG_SIZE, fp);
    }

    // Write Activity message.
    {
        FIT_ACTIVITY_MESG activity_mesg {};
        Fit_InitMesg(fit_mesg_defs[FIT_MESG_ACTIVITY], &activity_mesg);

        activity_mesg.timestamp = unixToFitTimestamp(track.timeStart);
        activity_mesg.local_timestamp = unixToFitTimestamp(epochToLocal(track.timeStart));
        activity_mesg.total_timer_time = static_cast<FIT_UINT32>((track.duration) * 1000);   // 1000 * s + 0, Exclude pauses
        activity_mesg.num_sessions = 1;

        WriteMessageDefinition(skActivityMsgNum, fit_mesg_defs[FIT_MESG_ACTIVITY], FIT_ACTIVITY_MESG_DEF_SIZE, fp);
        WriteMessage(skActivityMsgNum, &activity_mesg, FIT_ACTIVITY_MESG_SIZE, fp);
    }

    // Write CRC.
    WriteCRC(fp);

    // Update file header with data size.
    WriteFileHeader(fp);

    saveFile();
}

void ActivityWriter::discard()
{
    deleteFile();
}

bool ActivityWriter::createAndOpenFile(std::time_t utc)
{
    char buff[256]{};
    std::tm localTime{};
#if WIN32
    localtime_s(&localTime, &utc);
#else
    localtime_r(&utc, &localTime);
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

    mFile = mKernel.fs.file(buff);
    if (!mFile || !mFile->open(true, true)) { // write mode, override
        LOG_ERROR("Failed to create file [%s]\n", buff);
        mFile.reset();
        return false;
    }

    return true;
}

void ActivityWriter::saveFile()
{
    if (!mFile) {
        return;
    }

    mFile->flush();
    mFile->close();
}

void ActivityWriter::deleteFile()
{
    if (!mFile) {
        return;
    }

    if (mFile->isOpen()) {
        mFile->close();
    }

    mFile->remove();
    mFile.reset();
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

time_t ActivityWriter::epochToLocal(time_t utc)
{
    std::tm localTime{};
#if WIN32
    localtime_s(&localTime, &utc);
#else
    localtime_r(&utc, &localTime);
#endif
    return tm2epoch(&localTime);
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


// FIT-C

void ActivityWriter::WriteFileHeader(SDK::Interface::IFile* fp)
{
    FIT_FILE_HDR file_header {};

    file_header.header_size = FIT_FILE_HDR_SIZE;
    file_header.profile_version = FIT_PROFILE_VERSION;
    file_header.protocol_version = FIT_PROTOCOL_VERSION_20;
    memcpy((FIT_UINT8*)&file_header.data_type, ".FIT", 4);
    
    fp->flush();
    size_t fileSize = fp->size();

    if (fileSize > FIT_FILE_HDR_SIZE - sizeof(FIT_UINT16)) {
        file_header.data_size = static_cast<FIT_UINT32>(fileSize - FIT_FILE_HDR_SIZE - sizeof(FIT_UINT16));
    } else {
        file_header.data_size = 0;
    }

    file_header.crc = FitCRC_Calc16(&file_header, FIT_STRUCT_OFFSET(crc, FIT_FILE_HDR));

    fp->seek(0);  
    
    size_t bw;
    fp->write(reinterpret_cast<const char*>(&file_header), FIT_FILE_HDR_SIZE, bw);  

    fp->flush();

    // Move pointer to the end of the file
    if (fileSize > 0) {
        fp->seek(fileSize);
    }
    
}

void ActivityWriter::WriteData(const void* data, FIT_UINT16 data_size, SDK::Interface::IFile* fp)
{
    FIT_UINT16 offset;

    size_t bw;
    fp->write(reinterpret_cast<const char*>(data), static_cast<size_t>(data_size), bw);
    fp->flush();

    for (offset = 0; offset < data_size; offset++) {
        mDataCRC = FitCRC_Get16(mDataCRC, *((FIT_UINT8*)data + offset));
    }
}

void ActivityWriter::WriteCRC(SDK::Interface::IFile* fp)
{
    size_t bw;
    fp->write(reinterpret_cast<const char*>(&mDataCRC), sizeof(FIT_UINT16), bw);
    fp->flush();
}

void ActivityWriter::WriteMessageDefinition(FIT_UINT8 local_mesg_number, const void* mesg_def_pointer, FIT_UINT16 mesg_def_size, SDK::Interface::IFile* fp)
{
    FIT_UINT8 header = local_mesg_number | FIT_HDR_TYPE_DEF_BIT;
    WriteData(&header, FIT_HDR_SIZE, fp);
    WriteData(mesg_def_pointer, mesg_def_size, fp);
}

void ActivityWriter::WriteMessageDefinitionWithDevFields(FIT_UINT8 local_mesg_number, const void* mesg_def_pointer, FIT_UINT16 mesg_def_size,
    FIT_UINT8 number_dev_fields, FIT_DEV_FIELD_DEF* dev_field_definitions, SDK::Interface::IFile* fp)
{
    FIT_UINT16 i;
    FIT_UINT8 header = local_mesg_number | FIT_HDR_TYPE_DEF_BIT | FIT_HDR_DEV_DATA_BIT;
    WriteData(&header, FIT_HDR_SIZE, fp);
    WriteData(mesg_def_pointer, mesg_def_size, fp);

    WriteData(&number_dev_fields, sizeof(FIT_UINT8), fp);
    for (i = 0; i < number_dev_fields; i++) {
        WriteData(&dev_field_definitions[i], sizeof(FIT_DEV_FIELD_DEF), fp);
    }
}

void ActivityWriter::WriteMessage(FIT_UINT8 local_mesg_number, const void* mesg_pointer, FIT_UINT16 mesg_size, SDK::Interface::IFile* fp)
{
    WriteData(&local_mesg_number, FIT_HDR_SIZE, fp);
    WriteData(mesg_pointer, mesg_size, fp);
}

void ActivityWriter::WriteDeveloperField(const void* data, FIT_UINT16 data_size, SDK::Interface::IFile* fp)
{
    WriteData(data, data_size, fp);
}

