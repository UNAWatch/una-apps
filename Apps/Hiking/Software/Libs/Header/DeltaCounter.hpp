/**
 * @file DeltaCounter.hpp
 * @date 07-02-2025
 * @author Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief Delta counter for tracking metrics with positive and negative changes
 */

#ifndef DELTA_COUNTER_HPP
#define DELTA_COUNTER_HPP

#include <cstdint>
#include <limits>
#include <algorithm>

namespace SDK {

/**
 * @brief Delta counter for tracking cumulative changes
 * 
 * Tracks metrics that can both increase and decrease over time.
 * Suitable for: altitude, elevation.
 * 
 * Provides:
 * - Current value and delta from start
 * - Ascent (sum of positive changes) and Descent (sum of negative changes)
 * - Separate tracking for total activity and current lap
 * - Pause/resume support (affects ascent/descent only, not delta)
 * 
 * Note: This class does NOT perform filtering. Input values should be 
 * pre-filtered if needed (e.g., using moving average or other filters).
 */
class DeltaCounter {
public:
    /**
     * @brief Constructor - initializes delta counter
     */
    DeltaCounter();

    /**
     * @brief Destructor
     */
    ~DeltaCounter() = default;

    /**
     * @brief Initialize delta counter
     * 
     * @param minChange Minimum change threshold to register ascent/descent (e.g., 1.0m)
     *                  Changes smaller than this are ignored to reduce noise accumulation.
     *                  Must be non-negative.
     * @return true on success, false if minChange < 0
     */
    bool init(float minChange = 1.0f);

    /**
     * @brief Reset all calculations
     * 
     * Resets all values including current, delta, ascent, descent for both total and lap.
     */
    void reset();

    /**
     * @brief Reset current lap
     * 
     * Resets only lap-related values (delta, ascent, descent).
     */
    void resetLap();

    /**
     * @brief Add new measurement
     * 
     * First call after init/reset sets the start value.
     * Subsequent calls update current value, delta, and ascent/descent.
     * During pause, values are stored but ascent/descent are not updated.
     * 
     * @param currentValue Current metric value (e.g., altitude in meters)
     */
    void add(float currentValue);

    /**
     * @brief Get current value
     * 
     * @return Most recent value added, or 0.0 if no data
     */
    float getCurrent() const;

    /**
     * @brief Get delta from start
     * 
     * Delta is the difference between current value and start value.
     * Can be positive (net ascent) or negative (net descent).
     * Pause does not affect delta calculation.
     * 
     * @return Current value minus start value
     */
    float getDelta() const;

    /**
     * @brief Get lap delta from lap start
     * 
     * Delta for current lap (current value minus lap start value).
     * 
     * @return Current value minus lap start value
     */
    float getLapDelta() const;

    /**
     * @brief Get total ascent
     * 
     * Sum of all positive changes during active periods (excluding pauses).
     * Only changes >= minChange threshold are counted.
     * 
     * @return Total ascent in metric units
     */
    float getAscent() const;

    /**
     * @brief Get total descent
     * 
     * Sum of all negative changes during active periods (excluding pauses).
     * Only changes >= minChange threshold are counted.
     * Returns absolute value (always positive).
     * 
     * @return Total descent in metric units (absolute value)
     */
    float getDescent() const;

    /**
     * @brief Get current lap ascent
     * 
     * Sum of positive changes in current lap during active periods.
     * 
     * @return Lap ascent in metric units
     */
    float getLapAscent() const;

    /**
     * @brief Get current lap descent
     * 
     * Sum of negative changes in current lap during active periods.
     * Returns absolute value (always positive).
     * 
     * @return Lap descent in metric units (absolute value)
     */
    float getLapDescent() const;

    /**
     * @brief Pause tracking
     * 
     * Stops ascent/descent accumulation. Delta continues to update.
     * Use when activity is paused to avoid accumulating sensor noise.
     */
    void pause();

    /**
     * @brief Resume tracking
     * 
     * Resumes ascent/descent accumulation after pause.
     */
    void resume();

    /**
     * @brief Check if counter has valid data
     * 
     * @return true if at least one measurement has been added, false otherwise
     */
    bool isValid() const;

    /**
     * @brief Check if current lap has valid data
     * 
     * @return true if at least one measurement in current lap, false otherwise
     */
    bool isLapValid() const;

    /**
     * @brief Check if counter is currently paused
     * 
     * @return true if paused, false otherwise
     */
    bool isPaused() const;

private:
    float mMinChange;            /* Minimum change threshold for ascent/descent */
    
    float mCurrent;              /* Current value */
    float mStartValue;           /* Start value for delta calculation */
    float mLapStartValue;        /* Lap start value for lap delta calculation */
    
    float mAscent;               /* Total ascent (sum of positive changes) */
    float mDescent;              /* Total descent (sum of negative changes, absolute) */
    float mLapAscent;            /* Lap ascent */
    float mLapDescent;           /* Lap descent */
    
    float mLastValue;            /* Last value for change calculation */
    float mLapLastValue;         /* Last value for lap change calculation */
    
    bool mIsInitialized;         /* Initialization flag */
    bool mIsPaused;              /* Pause state flag */
    bool mHasData;               /* At least one data point received */
    bool mHasLapData;            /* At least one data point in current lap */
};

// Implementation

inline DeltaCounter::DeltaCounter()
    : mMinChange(1.0f)
    , mCurrent(0.0f)
    , mStartValue(0.0f)
    , mLapStartValue(0.0f)
    , mAscent(0.0f)
    , mDescent(0.0f)
    , mLapAscent(0.0f)
    , mLapDescent(0.0f)
    , mLastValue(0.0f)
    , mLapLastValue(0.0f)
    , mIsInitialized(false)
    , mIsPaused(false)
    , mHasData(false)
    , mHasLapData(false)
{
}

inline bool DeltaCounter::init(float minChange)
{
    /* Validate parameter */
    if (minChange < 0.0f) {
        return false;
    }

    mMinChange = minChange;
    mCurrent = 0.0f;
    mStartValue = 0.0f;
    mLapStartValue = 0.0f;
    mAscent = 0.0f;
    mDescent = 0.0f;
    mLapAscent = 0.0f;
    mLapDescent = 0.0f;
    mLastValue = 0.0f;
    mLapLastValue = 0.0f;
    mIsInitialized = true;
    mIsPaused = false;
    mHasData = false;
    mHasLapData = false;

    return true;
}

inline void DeltaCounter::reset()
{
    mCurrent = 0.0f;
    mStartValue = 0.0f;
    mLapStartValue = 0.0f;
    mAscent = 0.0f;
    mDescent = 0.0f;
    mLapAscent = 0.0f;
    mLapDescent = 0.0f;
    mLastValue = 0.0f;
    mLapLastValue = 0.0f;
    mIsPaused = false;
    mHasData = false;
    mHasLapData = false;
}

inline void DeltaCounter::resetLap()
{
    mLapStartValue = mCurrent;
    mLapAscent = 0.0f;
    mLapDescent = 0.0f;
    mLapLastValue = mCurrent;
    mHasLapData = false;
}

inline void DeltaCounter::add(float currentValue)
{
    if (!mIsInitialized) {
        return;
    }

    mCurrent = currentValue;

    /* Handle first measurement after init/reset */
    if (!mHasData) {
        mStartValue = currentValue;
        mLapStartValue = currentValue;
        mLastValue = currentValue;
        mLapLastValue = currentValue;
        mHasData = true;
        mHasLapData = true;
        return;
    }

    /* If paused, update current but don't accumulate ascent/descent */
    if (mIsPaused) {
        return;
    }

    /* Calculate changes from last registered points */
    float change = currentValue - mLastValue;
    float lapChange = currentValue - mLapLastValue;

    /* Update total ascent/descent if accumulated change exceeds threshold */
    if (change >= mMinChange) {
        mAscent += change;
        mLastValue = currentValue;  /* Update base for next threshold check */
    } else if (change <= -mMinChange) {
        mDescent += (-change);  /* Store as positive value */
        mLastValue = currentValue;  /* Update base for next threshold check */
    }
    /* If abs(change) < mMinChange, keep accumulating - don't update mLastValue */

    /* Update lap ascent/descent if accumulated change exceeds threshold */
    if (lapChange >= mMinChange) {
        mLapAscent += lapChange;
        mLapLastValue = currentValue;  /* Update lap base */
    } else if (lapChange <= -mMinChange) {
        mLapDescent += (-lapChange);  /* Store as positive value */
        mLapLastValue = currentValue;  /* Update lap base */
    }
    /* If abs(lapChange) < mMinChange, keep accumulating - don't update mLapLastValue */
}

inline float DeltaCounter::getCurrent() const
{
    return mCurrent;
}

inline float DeltaCounter::getDelta() const
{
    if (!mHasData) {
        return 0.0f;
    }
    return mCurrent - mStartValue;
}

inline float DeltaCounter::getLapDelta() const
{
    if (!mHasLapData) {
        return 0.0f;
    }
    return mCurrent - mLapStartValue;
}

inline float DeltaCounter::getAscent() const
{
    return mAscent;
}

inline float DeltaCounter::getDescent() const
{
    return mDescent;
}

inline float DeltaCounter::getLapAscent() const
{
    return mLapAscent;
}

inline float DeltaCounter::getLapDescent() const
{
    return mLapDescent;
}

inline void DeltaCounter::pause()
{
    if (!mIsInitialized || mIsPaused) {
        return;
    }

    mIsPaused = true;
}

inline void DeltaCounter::resume()
{
    if (!mIsInitialized || !mIsPaused) {
        return;
    }

    mIsPaused = false;
}

inline bool DeltaCounter::isValid() const
{
    return mHasData;
}

inline bool DeltaCounter::isLapValid() const
{
    return mHasLapData;
}

inline bool DeltaCounter::isPaused() const
{
    return mIsPaused;
}

}  // namespace SDK

#endif /* DELTA_COUNTER_HPP */
