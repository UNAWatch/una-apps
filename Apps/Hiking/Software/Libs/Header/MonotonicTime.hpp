/**
 * @file MonotonicTime.hpp
 * @date 07-02-2025
 * @author Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief Monotonic time tracker independent of RTC and timezone changes
 */

#ifndef MONOTONIC_TIME_HPP
#define MONOTONIC_TIME_HPP

#include <cstdint>
#include <ctime>
#include "SDK/Kernel/Kernel.hpp"

namespace SDK {

/**
 * @brief Monotonic time tracker
 * 
 * Provides monotonically increasing time values independent of RTC (Real-Time Clock)
 * or timezone changes. Uses system monotonic tick counter as reference.
 * 
 * Expected methods (monotonic):
 * - Based on initial timestamp + elapsed ticks
 * - Unaffected by RTC adjustments or timezone changes
 * - Guaranteed to be monotonically increasing
 * 
 * System methods (non-monotonic):
 * - Reflect current system time
 * - Affected by RTC and timezone changes
 */
class MonotonicTime {
public:
    /**
     * @brief Constructor
     * 
     * @param kernel Reference to SDK kernel for accessing system tick counter
     */
    explicit MonotonicTime(Kernel& kernel);

    /**
     * @brief Destructor
     */
    ~MonotonicTime() = default;

    /**
     * @brief Initialize starting timestamps for monotonic tracking
     * 
     * Captures current UTC time, local time offset, and monotonic tick count.
     * Must be called before using other methods.
     */
    void init();

    /**
     * @brief Get expected UTC time based on monotonic counter
     * 
     * Calculates UTC time using initial timestamp and elapsed monotonic ticks.
     * Result is monotonically increasing and unaffected by RTC changes.
     * 
     * @return Expected UTC time (seconds since epoch)
     */
    std::time_t getExpectedUTC();

    /**
     * @brief Get expected local time for current monotonic time
     * 
     * Calculates local time using initial offset and elapsed monotonic ticks.
     * Result is monotonically increasing and unaffected by RTC or timezone changes.
     * 
     * @return Expected local time as tm structure
     */
    std::tm getExpectedLocalTime();

    /**
     * @brief Get expected local time for given UTC
     * 
     * Converts given UTC to local time using initial timezone offset.
     * Unaffected by timezone changes after init().
     * 
     * @param utc UTC time to convert
     * @return Corresponding local time as tm structure
     */
    std::tm getExpectedLocalTime(std::time_t utc);

    /**
     * @brief Get current system local time for given UTC
     * 
     * Converts UTC to local time using current system timezone settings.
     * Reflects real-time timezone changes (non-monotonic behavior).
     * 
     * @param utc UTC time to convert
     * @return Corresponding system local time as tm structure
     */
    std::tm getLocalTime(std::time_t utc);

    /**
     * @brief Get elapsed time since initialization
     * 
     * @return Elapsed time in seconds since init() was called
     */
    uint64_t getElapsedSeconds();

    /**
     * @brief Get starting UTC timestamp
     * 
     * @return UTC time captured during init() call
     */
    std::time_t getStartUTC() const;

private:
    Kernel& mKernel;                    /* Reference to kernel for tick counter */
    std::time_t mStartUTC;              /* UTC time at init (seconds since epoch) */
    std::time_t mStartLocalTimeOffset;  /* Local time offset at init (seconds) */
    uint32_t mStartMono;                /* Monotonic tick count at init (milliseconds) */
    bool mIsInitialized;                /* Initialization flag */
};

// Implementation

inline MonotonicTime::MonotonicTime(Kernel& kernel)
    : mKernel(kernel)
    , mStartUTC(0)
    , mStartLocalTimeOffset(0)
    , mStartMono(0)
    , mIsInitialized(false)
{
}

inline void MonotonicTime::init()
{
    // Get UTC
    mStartUTC = std::time(nullptr);

    // Get local time offset
    std::tm gmt {};
    std::tm loc {};

#if defined(_WIN32) || defined(_WIN64)
    gmtime_s(&gmt, &mStartUTC);
    localtime_s(&loc, &mStartUTC);
#else
    gmtime_r(&mStartUTC, &gmt);
    localtime_r(&mStartUTC, &loc);
#endif

    std::time_t gmtTime = std::mktime(&gmt);
    std::time_t locTime = std::mktime(&loc);
    mStartLocalTimeOffset = static_cast<std::time_t>(std::difftime(locTime, gmtTime));

    // Get system monotonic tick count
    mStartMono = mKernel.sys.getTimeMs();

    mIsInitialized = true;
}

inline std::time_t MonotonicTime::getExpectedUTC()
{
    if (!mIsInitialized) {
        return 0;
    }

    uint32_t currentMono = mKernel.sys.getTimeMs();
    uint32_t elapsedMs = currentMono - mStartMono;
    
    return mStartUTC + (elapsedMs / 1000);
}

inline std::tm MonotonicTime::getExpectedLocalTime()
{
    return getExpectedLocalTime(getExpectedUTC());
}

inline std::tm MonotonicTime::getExpectedLocalTime(std::time_t utc)
{
    std::tm tmResult {};
    std::time_t localTime = utc + mStartLocalTimeOffset;

#if defined(_WIN32) || defined(_WIN64)
    gmtime_s(&tmResult, &localTime);
#else
    gmtime_r(&localTime, &tmResult);
#endif

    return tmResult;
}

inline std::tm MonotonicTime::getLocalTime(std::time_t utc)
{
    std::tm tmResult {};

#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tmResult, &utc);
#else
    localtime_r(&utc, &tmResult);
#endif

    return tmResult;
}

inline uint64_t MonotonicTime::getElapsedSeconds()
{
    if (!mIsInitialized) {
        return 0;
    }

    uint32_t currentMono = mKernel.sys.getTimeMs();
    uint32_t elapsedMs = currentMono - mStartMono;
    
    return elapsedMs / 1000;
}

inline std::time_t MonotonicTime::getStartUTC() const
{
    return mStartUTC;
}

}  // namespace SDK

#endif /* MONOTONIC_TIME_HPP */
