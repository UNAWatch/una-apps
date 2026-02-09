/**
 * @file MonotonicCounter.hpp
 * @date 06-02-2025
 * @author Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief Monotonic counter template for tracking cumulative metrics
 */

#ifndef MONOTONIC_COUNTER_HPP
#define MONOTONIC_COUNTER_HPP

#include <cstdint>
#include <type_traits>

namespace SDK {

/**
 * @brief Universal monotonic counter template for activity tracking
 *
 * Tracks cumulative metrics that monotonically increase over time.
 * Suitable for: distance, steps, floors, calories, time duration.
 * NOT suitable for: speed, heart rate, altitude (non-monotonic).
 *
 * Provides both active values (excluding pauses) and total values (including pauses).
 * Handles pause/resume to avoid sensor jumps affecting calculations.
 * Ensures total value equals sum of all lap values.
 *
 * @tparam T Arithmetic type for counter values (int, uint32_t, float, double, etc.)
 */
template<typename T>
class MonotonicCounter {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");

public:
    /**
     * @brief Constructor - initializes monotonic counter
     */
    MonotonicCounter();

    /**
     * @brief Destructor
     */
    ~MonotonicCounter() = default;

    /**
     * @brief Initialize monotonic counter
     *
     * Reserved for future filter parameters and thresholds.
     * Currently resets all state.
     */
    void init();

    /**
     * @brief Reset all value calculations
     *
     * Resets both total and lap values (active and total) to zero.
     * Next add() call will set new base point.
     */
    void reset();

    /**
     * @brief Reset current lap value
     *
     * Resets only lap value counters (active and total).
     * Next add() call will set new lap base point.
     */
    void resetLap();

    /**
     * @brief Add new metric measurement
     *
     * First call after init/reset sets base point for total value.
     * First call after resetLap sets base point for lap value.
     * Handles pause state by tracking both active and total values.
     * Ignores measurements that decrease compared to previous value (sensor errors).
     *
     * @param currentValue Current absolute metric value from sensor
     */
    void add(T currentValue);

    /**
     * @brief Get total active value
     *
     * Active value excludes time/metric accumulated during pauses.
     * Use for metrics like active time, active calories, moving distance.
     *
     * @return Total active value from start point
     */
    T getValueActive() const;

    /**
     * @brief Get total value including pauses
     *
     * Total value includes all time/metric from start, including pauses.
     * Use for elapsed time, total duration.
     *
     * @return Total value from start point including pauses
     */
    T getValueTotal() const;

    /**
     * @brief Get current lap active value
     *
     * Active lap value excludes time/metric accumulated during pauses.
     *
     * @return Current lap active value
     */
    T getLapValueActive() const;

    /**
     * @brief Get current lap total value including pauses
     *
     * Total lap value includes all time/metric from lap start, including pauses.
     *
     * @return Current lap total value including pauses
     */
    T getLapValueTotal() const;

    /**
     * @brief Get current/last received value
     *
     * Returns the most recent value added via add() method.
     *
     * @return Most recent value added, or T{} if no data
     */
    T getCurrent() const;

    /**
     * @brief Pause metric tracking
     *
     * Stops active value accumulation. Total value continues to accumulate.
     * Use before expected sensor jumps or when activity is paused.
     */
    void pause();

    /**
     * @brief Resume metric tracking
     *
     * Resumes active value accumulation after pause.
     * Next add() call will adjust base points to avoid jumps in active values.
     */
    void resume();

    /**
     * @brief Check if counter has valid data
     *
     * Returns true if at least one measurement has been added after init/reset.
     * Use to determine if counter values are meaningful.
     *
     * @return true if counter has received at least one data point, false otherwise
     */
    bool isValid() const;

    /**
     * @brief Check if current lap has valid data
     *
     * Returns true if at least one measurement has been added after last lap reset.
     * Use to determine if lap values are meaningful.
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
    T mValueActive;          /* Active value from start point (excluding pauses) */
    T mValueTotal;           /* Total value from start point (including pauses) */
    T mLapValueActive;       /* Current lap active value (excluding pauses) */
    T mLapValueTotal;        /* Current lap total value (including pauses) */
    T mBaseValue;            /* Base value for active calculations */
    T mBaseValueTotal;       /* Base value for total calculations */
    T mLapBaseValue;         /* Base value for lap active calculations */
    T mLapBaseValueTotal;    /* Base value for lap total calculations */
    bool mIsInitialized;     /* Counter initialization flag */
    bool mIsPaused;          /* Pause state flag */
    bool mHasData;           /* Flag indicating at least one data point received */
    bool mHasLapData;        /* Flag indicating at least one data point in current lap */
    T mLastValidValue;       /* Last valid value received */
    T mPauseStartValue;      /* Value when pause started (for total tracking) */
};

// Template implementation

template<typename T>
MonotonicCounter<T>::MonotonicCounter()
    : mValueActive(T{})
    , mValueTotal(T{})
    , mLapValueActive(T{})
    , mLapValueTotal(T{})
    , mBaseValue(T{})
    , mBaseValueTotal(T{})
    , mLapBaseValue(T{})
    , mLapBaseValueTotal(T{})
    , mIsInitialized(false)
    , mIsPaused(false)
    , mHasData(false)
    , mHasLapData(false)
    , mLastValidValue(T{})
    , mPauseStartValue(T{})
{
}

template<typename T>
void MonotonicCounter<T>::init()
{
    mValueActive = T{};
    mValueTotal = T{};
    mLapValueActive = T{};
    mLapValueTotal = T{};
    mBaseValue = T{};
    mBaseValueTotal = T{};
    mLapBaseValue = T{};
    mLapBaseValueTotal = T{};
    mIsInitialized = true;
    mIsPaused = false;
    mHasData = false;
    mHasLapData = false;
    mLastValidValue = T{};
    mPauseStartValue = T{};
}

template<typename T>
void MonotonicCounter<T>::reset()
{
    mValueActive = T{};
    mValueTotal = T{};
    mLapValueActive = T{};
    mLapValueTotal = T{};
    mBaseValue = T{};
    mBaseValueTotal = T{};
    mLapBaseValue = T{};
    mLapBaseValueTotal = T{};
    mLastValidValue = T{};
    mPauseStartValue = T{};
    mIsPaused = false;
    mHasData = false;
    mHasLapData = false;
}

template<typename T>
void MonotonicCounter<T>::resetLap()
{
    mLapValueActive = T{};
    mLapValueTotal = T{};
    mLapBaseValue = T{};
    mLapBaseValueTotal = T{};
    mHasLapData = false;
}

template<typename T>
void MonotonicCounter<T>::add(T currentValue)
{
    if (!mIsInitialized) {
        return;
    }

    /* Handle first measurement after init/reset */
    if (mBaseValue == T{} && mValueActive == T{}) {
        mBaseValue = currentValue;
        mBaseValueTotal = currentValue;
        mLapBaseValue = currentValue;
        mLapBaseValueTotal = currentValue;
        mLastValidValue = currentValue;
        mHasData = true;
        mHasLapData = true;
        return;
    }

    /* Handle first measurement after lap reset */
    if (mLapBaseValue == T{} && mLapValueActive == T{}) {
        mLapBaseValue = currentValue;
        mLapBaseValueTotal = currentValue;
        mHasLapData = true;
    }

    /* Ignore decreasing values - indicates sensor bug or error */
    if (currentValue < mLastValidValue) {
        return;
    }

    /* Always update total values (including during pause) */
    mValueTotal = currentValue - mBaseValueTotal;
    mLapValueTotal = currentValue - mLapBaseValueTotal;

    /* If paused, store value and skip active calculations */
    if (mIsPaused) {
        mLastValidValue = currentValue;
        return;
    }

    /* Calculate and update active values */
    mValueActive = currentValue - mBaseValue;
    mLapValueActive = currentValue - mLapBaseValue;
    mLastValidValue = currentValue;
}

template<typename T>
T MonotonicCounter<T>::getValueActive() const
{
    return mValueActive;
}

template<typename T>
T MonotonicCounter<T>::getValueTotal() const
{
    return mValueTotal;
}

template<typename T>
T MonotonicCounter<T>::getLapValueActive() const
{
    return mLapValueActive;
}

template<typename T>
T MonotonicCounter<T>::getLapValueTotal() const
{
    return mLapValueTotal;
}

template<typename T>
T MonotonicCounter<T>::getCurrent() const
{
    return mLastValidValue;
}

template<typename T>
void MonotonicCounter<T>::pause()
{
    if (!mIsInitialized || mIsPaused) {
        return;
    }

    mIsPaused = true;
    mPauseStartValue = mLastValidValue;
}

template<typename T>
void MonotonicCounter<T>::resume()
{
    if (!mIsInitialized || !mIsPaused) {
        return;
    }

    /* Calculate how much value changed during pause */
    T pauseOffset = mLastValidValue - mPauseStartValue;

    /* Adjust base points to exclude the paused change from active values */
    mBaseValue = mBaseValue + pauseOffset;
    mLapBaseValue = mLapBaseValue + pauseOffset;

    mIsPaused = false;
}

template<typename T>
bool MonotonicCounter<T>::isValid() const
{
    return mHasData;
}

template<typename T>
bool MonotonicCounter<T>::isLapValid() const
{
    return mHasLapData;
}

template<typename T>
bool MonotonicCounter<T>::isPaused() const
{
    return mIsPaused;
}

}  // namespace SDK

#endif /* MONOTONIC_COUNTER_HPP */
