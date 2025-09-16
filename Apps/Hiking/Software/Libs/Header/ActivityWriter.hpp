/**
 ******************************************************************************
 * @file    ActivityWriter.hpp
 * @date    31-08-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Serializes activity data to a FIT file.
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef __ACTIVITY_WRITER_HPP
#define __ACTIVITY_WRITER_HPP

#include <cstdint>
#include <cstdbool>
#include <string>

#include "SDK/Interfaces/IKernel.hpp"

extern "C" {
#include "fit_example.h"
}

/**
 * @class ActivityWriter
 * @brief Serializes activity data to a FIT file.
 */
class ActivityWriter {

public:

    struct AppInfo {
        std::time_t timestamp;  // UTC
        uint32_t appVersion;    // Application version 4 bytes LE [patch, minor, major, 0]
        std::string devID;      // Developer ID (max len 16)
        std::string appID;      // Application ID (max len 16)
    };

    struct RecordData {
        std::time_t timestamp;  // UTC
        float latitude;         // degrees
        float longitude;        // degrees
        uint8_t heartRate;      // Heart rate in beats per minute.
        uint32_t steps;
        uint32_t floors;
    };

    struct LapData {
        std::time_t timestamp;  // UTC
        std::time_t timeStart;  // UTC
        std::time_t duration;   // seconds
        std::time_t elapsed;    // seconds
        float distance;         // m
        float speedAvg;         // m/s
        float speedMax;         // m/s
        uint8_t hrAvg;          // bpm
        uint8_t hrMax;          // bpm
        float ascent;           // m
        float descent;          // m
        uint32_t steps;
        uint32_t floors;
    };

    struct TrackData {
        std::time_t timeStart;  // UTC
        std::time_t duration;   // seconds
        std::time_t elapsed;    // seconds
        float totalDistance;    // m
        float speedAvg;         // m/s
        float speedMax;         // m/s
        uint8_t hrAvg;          // bpm
        uint8_t hrMax;          // bpm
        float ascent;           // m
        float descent;          // m
        uint32_t steps;
        uint32_t floors;
    };


    ActivityWriter(const IKernel& kernel, const char* pathToDir);


    void start(const AppInfo& info);
    void pause();
    void resume();
    void addRecord(const RecordData& record);
    void addLap(const LapData& lap);
    void stop(const TrackData& track);
    void discard();

    
private:
    /// A constant reference to an IKernel object.
    const IKernel& mKernel;

    /// Path to FIT file
    const char* mPath = nullptr;

    std::unique_ptr<SDK::Interface::IFile> mFile = nullptr;
    uint16_t mLapCounter = 0;
    FIT_UINT16 mDataCRC = 0;

    static constexpr uint8_t skFileMsgNum = 1;
    static constexpr uint8_t skDevelopMsgNum = 2;
    static constexpr uint8_t skRecordMsgNum = 3;
    static constexpr uint8_t skLapMsgNum = 4;
    static constexpr uint8_t skSessionMsgNum = 5;
    static constexpr uint8_t skActivityMsgNum = 6;
    static constexpr uint8_t skEventMsgNum = 7;
    static constexpr uint8_t skStepsMsgNum = 8;
    static constexpr uint8_t skFloorsMsgNum = 9;



    bool createAndOpenFile(std::time_t utc);
    void saveFile();
    void deleteFile();

    static time_t tm2epoch(const struct tm* tm);
    static time_t epochToLocal(time_t utc);
    static FIT_DATE_TIME unixToFitTimestamp(std::time_t unixTimestamp);
    static FIT_SINT32 ConvertDegreesToSemicircles(float degrees);

    void WriteFileHeader(SDK::Interface::IFile* fp);
    void WriteMessageDefinition(FIT_UINT8 local_mesg_number, const void* mesg_def_pointer, FIT_UINT16 mesg_def_size, SDK::Interface::IFile* fp); 
    void WriteMessageDefinitionWithDevFields(FIT_UINT8 local_mesg_number, const void* mesg_def_pointer, FIT_UINT16 mesg_def_size,
        FIT_UINT8 number_dev_fields, FIT_DEV_FIELD_DEF* dev_field_definitions, SDK::Interface::IFile* fp);
    void WriteMessage(FIT_UINT8 local_mesg_number, const void* mesg_pointer, FIT_UINT16 mesg_size, SDK::Interface::IFile* fp);
    void WriteDeveloperField(const void* data, FIT_UINT16 data_size, SDK::Interface::IFile* fp);
    void WriteData(const void* data, FIT_UINT16 data_size, SDK::Interface::IFile* fp);
    void WriteCRC(SDK::Interface::IFile* fp);

};

#endif /* __ACTIVITY_WRITER_HPP */
