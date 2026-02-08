/**
 * @file VariableCounter.cpp
 * @date 06-02-2025
 * @author Denys <denys@example.com>
 * @brief Variable counter implementation
 */

#include "VariableCounter.hpp"
#include <algorithm>

VariableCounter::VariableCounter()
    : mMinValid(0.0f)
    , mMaxValid(0.0f)
    , mSumActive(0.0f)
    , mSumTotal(0.0f)
    , mCountActive(0)
    , mCountTotal(0)
    , mLapSumActive(0.0f)
    , mLapSumTotal(0.0f)
    , mLapCountActive(0)
    , mLapCountTotal(0)
    , mMinValue(std::numeric_limits<float>::max())
    , mMaxValue(0.0f)
    , mLapMinValue(std::numeric_limits<float>::max())
    , mLapMaxValue(0.0f)
    , mCurrentValue(0.0f)
    , mIsInitialized(false)
    , mIsPaused(false)
    , mHasData(false)
    , mHasLapData(false)
{}

bool VariableCounter::init(float minValid, float maxValid)
{
    /* Validate range */
    if (minValid >= maxValid) {
        return false;
    }

    mMinValid = minValid;
    mMaxValid = maxValid;
    mSumActive = 0.0f;
    mSumTotal = 0.0f;
    mCountActive = 0;
    mCountTotal = 0;
    mLapSumActive = 0.0f;
    mLapSumTotal = 0.0f;
    mLapCountActive = 0;
    mLapCountTotal = 0;
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

void VariableCounter::reset()
{
    mSumActive = 0.0f;
    mSumTotal = 0.0f;
    mCountActive = 0;
    mCountTotal = 0;
    mLapSumActive = 0.0f;
    mLapSumTotal = 0.0f;
    mLapCountActive = 0;
    mLapCountTotal = 0;
    mMinValue = std::numeric_limits<float>::max();
    mMaxValue = 0.0f;
    mLapMinValue = std::numeric_limits<float>::max();
    mLapMaxValue = 0.0f;
    mCurrentValue = 0.0f;
    mIsPaused = false;
    mHasData = false;
    mHasLapData = false;
}

void VariableCounter::resetLap()
{
    mLapSumActive = 0.0f;
    mLapSumTotal = 0.0f;
    mLapCountActive = 0;
    mLapCountTotal = 0;
    mLapMinValue = std::numeric_limits<float>::max();
    mLapMaxValue = 0.0f;
    mHasLapData = false;
}

void VariableCounter::add(float currentValue)
{
    if (!mIsInitialized) {
        return;
    }

    mCurrentValue = currentValue;

    /* Check if value is within valid range */
    if (currentValue < mMinValid || currentValue > mMaxValid) {
        return;
    }

    mHasData = true;
    mHasLapData = true;

    /* Update min/max with valid values only */
    mMinValue = std::min(mMinValue, currentValue);
    mMaxValue = std::max(mMaxValue, currentValue);
    mLapMinValue = std::min(mLapMinValue, currentValue);
    mLapMaxValue = std::max(mLapMaxValue, currentValue);

    /* Always update total statistics */
    mSumTotal += currentValue;
    mCountTotal++;
    mLapSumTotal += currentValue;
    mLapCountTotal++;

    /* Update active statistics only when not paused */
    if (!mIsPaused) {
        mSumActive += currentValue;
        mCountActive++;
        mLapSumActive += currentValue;
        mLapCountActive++;
    }
}

float VariableCounter::getAverageActive() const
{
    if (mCountActive == 0) {
        return 0.0f;
    }
    return mSumActive / static_cast<float>(mCountActive);
}

float VariableCounter::getAverageTotal() const
{
    if (mCountTotal == 0) {
        return 0.0f;
    }
    return mSumTotal / static_cast<float>(mCountTotal);
}

float VariableCounter::getLapAverageActive() const
{
    if (mLapCountActive == 0) {
        return 0.0f;
    }
    return mLapSumActive / static_cast<float>(mLapCountActive);
}

float VariableCounter::getLapAverageTotal() const
{
    if (mLapCountTotal == 0) {
        return 0.0f;
    }
    return mLapSumTotal / static_cast<float>(mLapCountTotal);
}

float VariableCounter::getMinimum() const
{
    if (!mHasData || mMinValue == std::numeric_limits<float>::max()) {
        return 0.0f;
    }
    return mMinValue;
}

float VariableCounter::getMaximum() const
{
    if (!mHasData) {
        return 0.0f;
    }
    return mMaxValue;
}

float VariableCounter::getLapMinimum() const
{
    if (mLapMinValue == std::numeric_limits<float>::max()) {
        return 0.0f;
    }
    return mLapMinValue;
}

float VariableCounter::getLapMaximum() const
{
    return mLapMaxValue;
}

float VariableCounter::getCurrent() const
{
    return mCurrentValue;
}

void VariableCounter::pause()
{
    if (!mIsInitialized || mIsPaused) {
        return;
    }

    mIsPaused = true;
}

void VariableCounter::resume()
{
    if (!mIsInitialized || !mIsPaused) {
        return;
    }

    mIsPaused = false;
}

bool VariableCounter::isValid() const
{
    return mHasData;
}

bool VariableCounter::isLapValid() const
{
    return mHasLapData;
}

bool VariableCounter::isPaused() const
{
    return mIsPaused;
}