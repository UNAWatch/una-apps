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

#include "SDK/Kernel/Kernel.hpp"
#include "SDK/FitHelper/FitHelper.hpp"

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
        bool        gotFix;
        float       latitude;   // degrees
        float       longitude;  // degrees
        float       altitude;   // absolute altitude in m
        float       heartRate;  // Heart rate in beats per minute.
		float       speed;      // m/s
    };              

    struct LapData {
        std::time_t timestamp;  // UTC
        std::time_t timeStart;  // UTC
        std::time_t duration;   // seconds
        std::time_t elapsed;    // seconds
        float distance;         // m
        float speedAvg;         // m/s
        float speedMax;         // m/s
        float hrAvg;            // bpm
        float hrMax;            // bpm
        float ascent;           // m
        float descent;          // m
        uint32_t steps;
        uint32_t floors;
    };

    struct TrackData {
        std::time_t timestamp;  // UTC
        std::time_t timeStart;  // UTC
        std::time_t duration;   // seconds
        std::time_t elapsed;    // seconds
        float distance;         // m
        float speedAvg;         // m/s
        float speedMax;         // m/s
        float hrAvg;            // bpm
        float hrMax;            // bpm
        float ascent;           // m
        float descent;          // m
        uint32_t steps;
        uint32_t floors;
    };

    ActivityWriter(const SDK::Kernel& kernel, const char* pathToDir);

    void start(const AppInfo& info);
    void pause(std::time_t timestamp);
    void resume(std::time_t timestamp);
    void addRecord(const RecordData& record);
    void addRecord(const RecordData& record, uint8_t battery);
    void addLap(const LapData& lap);
    void stop(const TrackData& track);
    void discard();

private:
    /// A constant reference to an Kernel object.
    const SDK::Kernel& mKernel;

    /// Path to FIT file
    const char* mPath = nullptr;

    std::unique_ptr<SDK::Interface::IFile> mFile = nullptr;
    uint16_t mLapCounter = 0;
    FIT_UINT16 mDataCRC = 0;

    SDK::Component::FitHelper mFHFileID;
    SDK::Component::FitHelper mFHDeveloper;
    SDK::Component::FitHelper mFHLap;
    SDK::Component::FitHelper mFHSession;
    SDK::Component::FitHelper mFHEvent;
    SDK::Component::FitHelper mFHActivity;
    SDK::Component::FitHelper mFHRecord;
    SDK::Component::FitHelper mFHRecordBattery;
    SDK::Component::FitHelper mFHStepsField;
    SDK::Component::FitHelper mFHFloorField;
    SDK::Component::FitHelper mFHBatteryField;

    static constexpr uint8_t skFileMsgNum          =  1;
    static constexpr uint8_t skDevelopMsgNum       =  2;
    static constexpr uint8_t skRecordMsgNum        =  3;
    static constexpr uint8_t skRecordBatteryMsgNum =  4;
    static constexpr uint8_t skLapMsgNum           =  5;
    static constexpr uint8_t skSessionMsgNum       =  6;
    static constexpr uint8_t skActivityMsgNum      =  7;
    static constexpr uint8_t skEventMsgNum         =  8;
    static constexpr uint8_t skStepsMsgNum         =  9;
    static constexpr uint8_t skFloorsMsgNum        = 10;
    static constexpr uint8_t skBatteryMsgNum       = 11;

    FIT_RECORD_MESG prepareRecordMsg(const RecordData& record);

    void AddMessageEvent(std::time_t t, FIT_EVENT_TYPE type);

    bool createAndOpenFile(std::time_t utc);
    void saveFile();
    void deleteFile();

    void saveSummary(const TrackData& track);

    static time_t tm2epoch(const struct tm* tm);
    static time_t epochToLocal(time_t utc);
    static FIT_DATE_TIME unixToFitTimestamp(std::time_t unixTimestamp);
    static FIT_SINT32 ConvertDegreesToSemicircles(float degrees);

    void WriteFileHeader(SDK::Interface::IFile* fp);
    void WriteCRC(SDK::Interface::IFile* fp);

};

#endif /* __ACTIVITY_WRITER_HPP */
