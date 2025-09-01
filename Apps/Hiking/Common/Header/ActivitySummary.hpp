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

#include "TrackMapScreen.hpp"

/**
 * @struct ActivitySummary
 * @brief Represents a common summary track information.
 */
struct ActivitySummary {
    time_t utc;         ///< Last activity UTC time
    time_t time;        ///< Total track time in seconds
    float distance;     ///< Total track distance in km
    float speedAvg;     ///< Average speed in km/h
    int32_t steps;      ///< Steps number if any
    int32_t elevation;  ///< Elevation in m
    int32_t paceAvg;    ///< Average pace in seconds/km
    int32_t hrMax;      ///< Maximum Heart Rate in bbs
    int32_t hrAvg;      ///< Average Heart Rate in bbs
    TrackMapScreen map; ///< Track map
};

#endif /* __ACTIVITY_SUMMARY_HPP */