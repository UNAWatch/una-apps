/**
 * @file VariableCounter.hpp
 * @date 06-02-2025
 * @author Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief Variable counter for tracking non-monotonic metrics
 */

#ifndef VARIABLE_COUNTER_HPP
#define VARIABLE_COUNTER_HPP

#include <cstdint>
#include <limits>

namespace SDK {

/**
 * @brief Universal variable counter for activity tracking
 *
 * Tracks metrics that can increase or decrease over time.
 * Suitable for: speed, heart rate, cadence, power, pace, altitude, temperature.
 * NOT suitable for: distance, steps, floors, calories (use MonotonicCounter).
 *
 * Provides average, minimum, maximum values for active periods (excluding pauses).
 *
 * Uses valid range [minValid, maxValid] to filter invalid sensor readings.
 * Values outside this range are excluded from all statistics.
 *
 * Assumes uniform sampling - add() should be called at regular intervals
 * for accurate average calculations.
 */
class VariableCounter {
public:
    /**
     * @brief Constructor - initializes variable counter
     */
    VariableCounter();

    /**
     * @brief Destructor
     */
    ~VariableCounter() = default;

    /**
     * @brief Initialize variable counter
     *
     * @param minValid Minimum valid value threshold. Values below are excluded from statistics.
     * @param maxValid Maximum valid value threshold. Values above are excluded from statistics.
     * @return true on success, false if minValid >= maxValid
     */
    bool init(float minValid, float maxValid);

    /**
     * @brief Reset all value calculations
     *
     * Resets both total and lap statistics (averages, min, max) to initial state.
     */
    void reset();

    /**
     * @brief Reset current lap statistics
     *
     * Resets only lap statistics (averages, min, max).
     */
    void resetLap();

    /**
     * @brief Add new metric measurement
     *
     * Should be called at regular intervals for accurate average calculation.
     * Values outside [minValid, maxValid] range are completely excluded from all statistics.
     *
     * @param currentValue Current metric value from sensor
     */
    void add(float currentValue);

    /**
     * @brief Get average value
     *
     * Average value excludes paused periods and values outside valid range.
     * Calculated only for active (non-paused) measurements.
     *
     * @return Average value during active periods, 0.0 if no valid samples
     */
    float getAverage() const;

    /**
     * @brief Get current lap average value
     *
     * Lap average excludes paused periods and values outside valid range.
     * Calculated only for active (non-paused) measurements.
     *
     * @return Average lap value during active periods, 0.0 if no valid samples
     */
    float getLapAverage() const;

    /**
     * @brief Get minimum value
     *
     * Returns minimum valid value observed during active periods (excluding pauses)
     * within [minValid, maxValid] range.
     *
     * @return Minimum value during active periods, or 0.0 if no valid data
     */
    float getMinimum() const;

    /**
     * @brief Get maximum value
     *
     * Returns maximum valid value observed during active periods (excluding pauses)
     * within [minValid, maxValid] range.
     *
     * @return Maximum value during active periods, or 0.0 if no valid data
     */
    float getMaximum() const;

    /**
     * @brief Get current lap minimum value
     *
     * Returns minimum valid value observed in current lap during active periods (excluding pauses)
     * within [minValid, maxValid] range.
     *
     * @return Lap minimum value during active periods, or 0.0 if no valid data
     */
    float getLapMinimum() const;

    /**
     * @brief Get current lap maximum value
     *
     * Returns maximum valid value observed in current lap during active periods (excluding pauses)
     * within [minValid, maxValid] range.
     *
     * @return Lap maximum value during active periods, or 0.0 if no valid data
     */
    float getLapMaximum() const;

    /**
     * @brief Get current/last received value
     *
     * @return Most recent value added, or 0.0 if no data
     */
    float getCurrent() const;

    /**
     * @brief Get minimum valid threshold
     *
     * @return Minimum valid value threshold set in init()
     */
    float getMinValid() const;

    /**
     * @brief Get maximum valid threshold
     *
     * @return Maximum valid value threshold set in init()
     */
    float getMaxValid() const;

    /**
     * @brief Pause metric tracking
     *
     * Stops all statistics accumulation during paused periods.
     * Use before expected sensor jumps or when activity is paused.
     */
    void pause();

    /**
     * @brief Resume metric tracking
     *
     * Resumes active statistics accumulation after pause.
     */
    void resume();

    /**
     * @brief Check if counter has valid data
     *
     * Returns true if at least one measurement has been added after init/reset.
     *
     * @return true if counter has received at least one data point, false otherwise
     */
    bool isValid() const;

    /**
     * @brief Check if current lap has valid data
     *
     * Returns true if at least one measurement has been added after last lap reset.
     *
     * @return true if lap has received at least one data point, false otherwise
     */
    bool isLapValid() const;

    /**
     * @brief Check if counter is currently paused
     *
     * @return true if counter is paused, false otherwise
     */
    bool isPaused() const;

private:
    float mMinValid;             /* Minimum valid value threshold */
    float mMaxValid;             /* Maximum valid value threshold */

    float mSumActive;            /* Sum of values during active periods */
    uint32_t mCountActive;       /* Count of valid samples during active periods */

    float mLapSumActive;         /* Sum of lap values during active periods */
    uint32_t mLapCountActive;    /* Count of valid lap samples during active periods */

    float mMinValue;             /* Minimum value observed during active periods */
    float mMaxValue;             /* Maximum value observed during active periods */
    float mLapMinValue;          /* Minimum lap value observed during active periods */
    float mLapMaxValue;          /* Maximum lap value observed during active periods */

    float mCurrentValue;         /* Most recent value */

    bool mIsInitialized;         /* Counter initialization flag */
    bool mIsPaused;              /* Pause state flag */
    bool mHasData;               /* Flag indicating at least one data point received */
    bool mHasLapData;            /* Flag indicating at least one data point in current lap */
};

// Implementation

inline VariableCounter::VariableCounter()
    : mMinValid(0.0f)
    , mMaxValid(0.0f)
    , mSumActive(0.0f)
    , mCountActive(0)
    , mLapSumActive(0.0f)
    , mLapCountActive(0)
    , mMinValue(std::numeric_limits<float>::max())
    , mMaxValue(0.0f)
    , mLapMinValue(std::numeric_limits<float>::max())
    , mLapMaxValue(0.0f)
    , mCurrentValue(0.0f)
    , mIsInitialized(false)
    , mIsPaused(false)
    , mHasData(false)
    , mHasLapData(false)
{
}

inline bool VariableCounter::init(float minValid, float maxValid)
{
    /* Validate range */
    if (minValid >= maxValid) {
        return false;
    }

    mMinValid = minValid;
    mMaxValid = maxValid;
    mSumActive = 0.0f;
    mCountActive = 0;
    mLapSumActive = 0.0f;
    mLapCountActive = 0;
    mMinValue = std::numeric_limits<float>::max();
    mMaxValue = 0.0f;
    mLapMinValue = std::numeric_limits<float>::max();
    mLapMaxValue = 0.0f;
    mCurrentValue = 0.0f;
    mIsInitialized = true;
    mIsPaused = false;
    mHasData = false;
    mHasLapData = false;

    return true;
}

inline void VariableCounter::reset()
{
    mSumActive = 0.0f;
    mCountActive = 0;
    mLapSumActive = 0.0f;
    mLapCountActive = 0;
    mMinValue = std::numeric_limits<float>::max();
    mMaxValue = 0.0f;
    mLapMinValue = std::numeric_limits<float>::max();
    mLapMaxValue = 0.0f;
    mCurrentValue = 0.0f;
    mIsPaused = false;
    mHasData = false;
    mHasLapData = false;
}

inline void VariableCounter::resetLap()
{
    mLapSumActive = 0.0f;
    mLapCountActive = 0;
    mLapMinValue = std::numeric_limits<float>::max();
    mLapMaxValue = 0.0f;
    mHasLapData = false;
}

inline void VariableCounter::add(float currentValue)
{
    if (!mIsInitialized) {
        return;
    }

    mCurrentValue = currentValue;

    /* Check if value is within valid range */
    if (currentValue < mMinValid || currentValue > mMaxValid) {
        return;
    }

    /* If paused, don't update any statistics */
    if (mIsPaused) {
        return;
    }

    mHasData = true;
    mHasLapData = true;

    /* Update min/max for active periods only */
    mMinValue = std::min(mMinValue, currentValue);
    mMaxValue = std::max(mMaxValue, currentValue);
    mLapMinValue = std::min(mLapMinValue, currentValue);
    mLapMaxValue = std::max(mLapMaxValue, currentValue);

    /* Update active statistics */
    mSumActive += currentValue;
    mCountActive++;
    mLapSumActive += currentValue;
    mLapCountActive++;
}

inline float VariableCounter::getAverage() const
{
    if (mCountActive == 0) {
        return 0.0f;
    }
    return mSumActive / static_cast<float>(mCountActive);
}

inline float VariableCounter::getLapAverage() const
{
    if (mLapCountActive == 0) {
        return 0.0f;
    }
    return mLapSumActive / static_cast<float>(mLapCountActive);
}

inline float VariableCounter::getMinimum() const
{
    if (!mHasData || mMinValue == std::numeric_limits<float>::max()) {
        return 0.0f;
    }
    return mMinValue;
}

inline float VariableCounter::getMaximum() const
{
    if (!mHasData) {
        return 0.0f;
    }
    return mMaxValue;
}

inline float VariableCounter::getLapMinimum() const
{
    if (mLapMinValue == std::numeric_limits<float>::max()) {
        return 0.0f;
    }
    return mLapMinValue;
}

inline float VariableCounter::getLapMaximum() const
{
    return mLapMaxValue;
}

inline float VariableCounter::getCurrent() const
{
    return mCurrentValue;
}

inline float VariableCounter::getMinValid() const
{
    return mMinValid;
}

inline float VariableCounter::getMaxValid() const
{
    return mMaxValid;
}

inline void VariableCounter::pause()
{
    if (!mIsInitialized || mIsPaused) {
        return;
    }

    mIsPaused = true;
}

inline void VariableCounter::resume()
{
    if (!mIsInitialized || !mIsPaused) {
        return;
    }

    mIsPaused = false;
}

inline bool VariableCounter::isValid() const
{
    return mHasData;
}

inline bool VariableCounter::isLapValid() const
{
    return mHasLapData;
}

inline bool VariableCounter::isPaused() const
{
    return mIsPaused;
}

}  // namespace SDK

#endif /* VARIABLE_COUNTER_HPP */
