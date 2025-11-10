/**
 ******************************************************************************
 * @file    ActivitySummary.hpp
 * @date    08-04-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Represents a common summary track information.
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef __ACTIVITY_SUMMARY_HPP
#define __ACTIVITY_SUMMARY_HPP

#include <cstdint>
#include <string>
#include <ctime>

#include "SDK/TrackMap/TrackMapScreen.hpp"

/**
 * @struct ActivitySummary
 * @brief Represents a common summary track information.
 */
struct ActivitySummary {
    time_t utc;         ///< Last activity UTC time
    time_t time;        ///< Total track time in seconds
    float distance;     ///< Total track distance in m
    float speedAvg;     ///< Average speed in m/s
    float elevation;    ///< Elevation in m
    float paceAvg;      ///< Average pace in s/m
    float hrMax;        ///< Maximum Heart Rate in bbs
    float hrAvg;        ///< Average Heart Rate in bbs
    SDK::TrackMapScreen map; ///< Track map
};

#endif /* __ACTIVITY_SUMMARY_HPP */
