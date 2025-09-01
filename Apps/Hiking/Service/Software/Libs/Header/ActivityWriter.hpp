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

#include "IKernel.hpp"
#include "fit_encode.hpp"

/**
 * @class ActivityWriter
 * @brief Serializes activity data to a FIT file.
 */
class ActivityWriter {

public:
    struct TrackData {
        uint32_t appVersion;    // Application version 4 bytes LE [patch, minor, major, 0]
        std::string devID;      // Developer ID (max len 16)
        std::string appID;      // Application ID (max len 16)

        std::time_t timeStart;  // UTC
        std::time_t duration;   // seconds
        std::time_t elapsed;    // seconds
        float totalDistance;    // m
        float speedAvg;         // m/s
        float speedMax;         // m/s
        uint8_t hrAvg;          // bpm
        uint8_t hrMax;          // bpm
        float elevation;        // m
        uint32_t steps;
        uint32_t floors;
    };


    struct LapData {
        std::time_t timeStart;  // UTC
        std::time_t duration;   // seconds
        std::time_t elapsed;    // seconds
        float distance;         // m
        float speedAvg;         // m/s
        float speedMax;         // m/s
        uint8_t hrAvg;          // bpm
        uint8_t hrMax;          // bpm
        float elevation;        // m
        uint32_t steps;
        uint32_t floors;
    };

    /**
     * @struct PointData
     * @brief Represents a single activity data point.
     */
    struct PointData {
        std::time_t timestamp;  // UTC
        float latitude;         // degrees
        float longitude;        // degrees
        uint8_t heartRate;      // Heart rate in beats per minute.
        uint32_t steps;
        uint32_t floors;
    };


    ActivityWriter(const IKernel& kernel, const char* pathToDir);

    bool createFile(const TrackData& track, const std::vector<LapData>& laps, const std::vector<PointData>& points);

    
private:
    /// A constant reference to an IKernel object.
    const IKernel& mKernel;

    /// Path to FIT file
    const char* mPath = nullptr;

    void fill(fit::Encode& encode, const TrackData& track, const std::vector<LapData>& laps, const std::vector<PointData>& points);
    static time_t tm2epoch(const struct tm* tm);
    static FIT_DATE_TIME unixToFitTimestamp(std::time_t unixTimestamp);
    static FIT_SINT32 ConvertDegreesToSemicircles(float degrees);
};

#endif /* __ACTIVITY_WRITER_HPP */
