/**
 ******************************************************************************
 * @file    TrackInfo.hpp
 * @date    08-04-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Represents current track information.
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef __TRACK_INFO_HPP
#define __TRACK_INFO_HPP

#include <cstdint>
#include <ctime>

namespace Track
{

/**
 * @enum Track::State
 * @brief Represents the state of a track.
 */
enum class State {
    INACTIVE = 0,
    ACTIVE,
    PAUSED
};

/**
 * @struct Track::Data
 * @brief Represents current track information.
 */
struct Data {

    // Pace in s/m
    float pace;
    float avgPace;
    float lapPace;

    // Distance in m
    float distance;
    float lapDistance;

    // Time in seconds
    time_t totalTime;
    time_t lapTime;

    uint32_t lapNum;

    float HR;
    float hrTrustLevel;
    float avgHR;
    float maxHR;
    float avgLapHR;
    float maxLapHR;

    // Speed in m/s
    float speed;
    float avgSpeed;
    float maxSpeed;
    float avgLapSpeed;
    float maxLapSpeed;

    // Elevation in m
    float elevation;
    float lapElevation;

    uint32_t steps;
    uint32_t lapSteps;

    uint32_t floors;
    uint32_t lapFloors;
};

} // namespace Track

#endif /* __TRACK_INFO_HPP */
