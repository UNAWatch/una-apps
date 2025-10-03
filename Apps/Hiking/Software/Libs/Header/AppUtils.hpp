/**
 ******************************************************************************
 * @file    AppUtils.hpp
 * @date    25-09-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Application utilities.
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef __APP_UTILS_HPP
#define __APP_UTILS_HPP

#include <cstdint>
#include <ctime>


namespace App::Utils
{

/**
 * @brief   Convert kilometers to miles.
 * @param   km: Value in kilometers.
 * @return  Value in miles.
 */
inline float km2mi(float km)
{
    return km * 0.621371f;
}

/**
 * @brief   Convert miles to kilometers.
 * @param   mi: Value in miles.
 * @return  Value in kilometers.
 */
inline float mi2km(float mi)
{
    return mi / 0.621371f;
}

/**
 * @brief   Convert meters to feet.
 * @param   m: Value in meters.
 * @return  Value in feet.
 */
inline float m2ft(float m)
{
    return m * 3.28084f;
}

/**
 * @brief   Convert feet to meters.
 * @param   ft: Value in feet.
 * @return  Value in meters.
 */
inline float ft2m(float ft)
{
    return ft / 3.28084f;
}

/**
 * @brief   Convert seconds to hours, minutes and seconds.
 * @param   sec: Seconds to convert.
 * @param   h: Reference to store hours.
 * @param   m: Reference to store minutes.
 * @param   s: Reference to store seconds.
 */
inline void sec2hms(std::time_t sec, uint16_t& h, uint8_t& m, uint8_t& s)
{
    h = static_cast<uint16_t>(sec / 3600);
    sec %= 3600;
    m = static_cast<uint8_t>(sec / 60);
    s = static_cast<uint8_t>(sec % 60);
}

/**
 * @brief   Convert seconds to hours.
 * @param   sec: Seconds to convert.
 * @return  Hours.
 */
inline uint16_t sec2hmsH(std::time_t sec)
{
    uint16_t h;
    uint8_t m;
    uint8_t s;
    sec2hms(sec, h, m, s);
    return h;
}

/**
 * @brief   Convert seconds to minutes.
 * @param   sec: Seconds to convert.
 * @return  Minutes.
 */
inline uint8_t sec2hmsM(std::time_t sec)
{
    uint16_t h;
    uint8_t m;
    uint8_t s;
    sec2hms(sec, h, m, s);
    return m;
}

/**
 * @brief   Convert seconds to seconds.
 * @param   sec: Seconds to convert.
 * @return  Seconds.
 */
inline uint8_t sec2hmsS(std::time_t sec)
{
    uint16_t h;
    uint8_t m;
    uint8_t s;
    sec2hms(sec, h, m, s);
    return s;
}

} // App::Utils

#endif /* __APP_UTILS_HPP */