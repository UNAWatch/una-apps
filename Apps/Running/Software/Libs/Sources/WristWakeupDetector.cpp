/*******************************************************************************
 * @file   WristWakeupDetector.cpp
 * @date   02-04-2025
 * @author Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief  Wrist wakeup gesture detector implementation.
 ******************************************************************************/

#include "WristWakeupDetector.hpp"

#include <cstring>
#include <cmath>

/*******************************************************************************
 * Construction / lifecycle
 ******************************************************************************/

WristWakeupDetector::WristWakeupDetector(const Config& cfg) noexcept
    : mCfg(cfg)
{
    mDt              = 1.0f / cfg.sampleRateHz;
    mCooldownSamples = static_cast<uint32_t>(
        cfg.cooldownS * cfg.sampleRateHz + 0.5f);

    /* Clamp preWin to ring buffer capacity. */
    if (mCfg.preWinSamples > MAX_PRE_WIN) {
        mCfg.preWinSamples = MAX_PRE_WIN;
    }

    reset();
}

void WristWakeupDetector::setListener(IListener* listener) noexcept {
    mListener = listener;
}

void WristWakeupDetector::reset() noexcept {
    mHead              = 0u;
    mFilled            = 0u;
    mGxSum            = 0.0f;
    mGxIntegralCache = 0.0f;
    mCooldownRemaining= 0u;
    mTotalEvents      = 0u;
    std::memset(mAzBuf, 0, sizeof(mAzBuf));
    std::memset(mGxBuf, 0, sizeof(mGxBuf));
}

/*******************************************************************************
 * Main processing
 ******************************************************************************/

bool WristWakeupDetector::addBatch(const ImuSample* samples,
                                    size_t           count,
                                    uint32_t         baseTimeMs) noexcept {
    if (samples == nullptr || count == 0u) {
        return false;
    }

    const float dtMs = mDt * 1000.0f;
    bool anyTriggered = false;

    for (size_t i = 0u; i < count; ++i) {
        const ImuSample& s = samples[i];

        /* 1. Push sample into ring buffer and update running GX integral. */
        pushSample(s);

        /* 2. Decrement cooldown. */
        if (mCooldownRemaining > 0u) {
            --mCooldownRemaining;
        }

        /* 3. Only evaluate when ring buffer is fully primed. */
        if (mFilled < mCfg.preWinSamples) {
            continue;
        }

        /* 4. Fast early exit: AZ below threshold — no chance of detection. */
        if (s.azG < mCfg.azThreshG) {
            continue;
        }

        /* 5. Compute GX integral (degrees) over the integration window. */
        const float gxInt = mGxSum * mDt;
        mGxIntegralCache = gxInt;

        /* 6. Evaluate detection condition and fire event if met. */
        const uint32_t ts_ms = baseTimeMs +
                               static_cast<uint32_t>(
                                   static_cast<float>(i) * dtMs + 0.5f);

        if (evaluateSample(s.azG, gxInt, ts_ms)) {
            anyTriggered = true;
        }
    }

    return anyTriggered;
}

/*******************************************************************************
 * Private helpers
 ******************************************************************************/

/**
 * @brief Push one sample into the ring buffer.
 *
 *        Maintains mGxSum as a running integral numerator (sum of GX values)
 *        so that computeGxIntegral() is O(1): integral = mGxSum * DT.
 *
 *        When the buffer is full, the oldest sample is subtracted from the
 *        sum before being overwritten — sliding window in O(1).
 */
void WristWakeupDetector::pushSample(const ImuSample& s) noexcept {
    const uint8_t win = mCfg.preWinSamples;

    if (mFilled >= win) {
        /* Remove the oldest GX value from the running sum. */
        mGxSum -= mGxBuf[mHead];
    } else {
        ++mFilled;
    }

    /* Write new sample at head position. */
    mAzBuf[mHead] = s.azG;
    mGxBuf[mHead] = s.gxDps;
    mGxSum        += s.gxDps;

    /* Advance head (circular). */
    mHead = static_cast<uint8_t>((mHead + 1u) % win);
}

/**
 * @brief Evaluate detection conditions for the current sample.
 * @return true if a gesture event was fired.
 */
bool WristWakeupDetector::evaluateSample(float    azG,
                                          float    gxIntegralDeg,
                                          uint32_t timestampMs) noexcept {
    /* Condition 1: cooldown elapsed. */
    if (mCooldownRemaining > 0u) {
        return false;
    }

    /* Condition 2: AZ above threshold (screen faces user). */
    if (azG < mCfg.azThreshG) {
        return false;
    }

    /* Condition 3: sustained wrist rotation in the configured direction.
     * Threshold sign determines required direction:
     *   positive threshold → integral must exceed  +threshold
     *   negative threshold → integral must be below -|threshold|
     * Both cases reduce to: sign(thresh) * integral < |thresh|  → reject */
    const float sign = (mCfg.gxIntThreshDeg >= 0.0f) ? 1.0f : -1.0f;
    if (sign * gxIntegralDeg < sign * mCfg.gxIntThreshDeg) {
        return false;
    }

    /* All conditions met — fire event. */
    mCooldownRemaining = mCooldownSamples;
    ++mTotalEvents;

    if (mListener != nullptr) {
        mListener->onWristWakeup(timestampMs);
    }

    return true;
}
