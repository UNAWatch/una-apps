/*******************************************************************************
 * @file   WristWakeupDetector.hpp
 * @date   02-04-2025
 * @author Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief  Custom wrist wakeup gesture detector for running activity.
 *
 *         Algorithm: detects wrist supination (watch-look gesture) by
 *         monitoring AZ (screen-toward-face) rise combined with sustained
 *         negative GX integral (wrist rotation along arm axis).
 *
 *         Tuned parameters (50 Hz input):
 *           AZ_THRESH      = 1.1 g    screen pointing toward face
 *           GX_INT_THRESH  = -12 deg  sustained supination in 400 ms window
 *           PRE_WIN        = 20 samp  integration window (400 ms at 50 Hz)
 *           COOLDOWN       = 3.0 s    minimum interval between events
 *
 *         Performance on running dataset (3 sessions, 111 real events):
 *           Recall ~100%  Precision ~30%  (false positives acceptable per spec)
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <cstddef>

/*******************************************************************************
 * @brief Single IMU sample — accelerometer + gyroscope, already converted to
 *        physical units before being passed to the detector.
 ******************************************************************************/
struct ImuSample {
    float azG;   /**< Accelerometer Z axis, g  (screen normal, toward face) */
    float gxDps; /**< Gyroscope X axis, deg/s  (roll along arm axis)        */
};

/*******************************************************************************
 * @brief Wrist wakeup gesture detector.
 *
 *        Usage:
 *          1. Instantiate once (or call reset() to reuse).
 *          2. Call addBatch() with each incoming batch of samples.
 *          3. Check the return value — true means gesture detected.
 *
 *        All configuration lives in Config; defaults match tuned values.
 ******************************************************************************/
class WristWakeupDetector {
public:
    /*--------------------------------------------------------------------------
     * Configuration
     *------------------------------------------------------------------------*/
    struct Config {
        /** Sample rate of input data, Hz. Used to compute DT and cooldown. */
        float   sampleRateHz    = 50.0f;

        /** AZ threshold above which screen is considered facing the user, g. */
        float   azThreshG       = 1.1f;

        /**
         * Signed GX integral threshold, degrees.
         *
         * The sign encodes the required rotation direction:
         *   Positive (+12.0) — integral must EXCEED   +threshold
         *   Negative (-12.0) — integral must FALL BELOW -|threshold|
         *
         * Correct sign depends on sensor mounting / axis remapping.
         * Larger absolute value = stricter (fewer FP, more misses).
         * Default +12.0 matches left-wrist BMI270 where the watch-look
         * gesture produces a positive GX integral.
         */
        float   gxIntThreshDeg = +12.0f;

        /**
         * Integration window length in samples.
         * 20 samples @ 50 Hz = 400 ms.
         */
        uint8_t preWinSamples   = 20;

        /** Minimum time between two consecutive events, seconds. */
        float   cooldownS        = 3.0f;
    };

    /*--------------------------------------------------------------------------
     * IListener — implement this to receive gesture events
     *------------------------------------------------------------------------*/
    class IListener {
    public:
        virtual ~IListener() = default;

        /**
         * @brief Called when a wrist wakeup gesture is detected.
         * @param timestampMs Timestamp of the triggering sample, ms.
         *                     Sourced from the sample passed to addBatch().
         */
        virtual void onWristWakeup(uint32_t timestampMs) = 0;
    };

    /*--------------------------------------------------------------------------
     * Construction / lifecycle
     *------------------------------------------------------------------------*/

    /**
     * @brief Construct with optional configuration.
     * @param cfg  Detector tuning parameters.
     */
    explicit WristWakeupDetector(const Config& cfg) noexcept;

    /** @brief Construct with default configuration. */
    WristWakeupDetector() noexcept : WristWakeupDetector(Config{}) {}

    /**
     * @brief Set or replace the event listener.
     * @param listener  Pointer to listener; nullptr disables callbacks.
     */
    void setListener(IListener* listener) noexcept;

    /**
     * @brief Reset internal state (ring buffer, cooldown counter).
     *        Configuration and listener are preserved.
     */
    void reset() noexcept;

    /*--------------------------------------------------------------------------
     * Main processing
     *------------------------------------------------------------------------*/

    /**
     * @brief Process one batch of IMU samples.
     *
     *        Call this every time a new batch arrives from the sensor FIFO
     *        (e.g. every 100 ms at 50 Hz → ~5 samples per batch).
     *        Samples must be in chronological order within the batch.
     *
     * @param samples      Pointer to array of ImuSample.
     * @param count        Number of samples in this batch.
     * @param baseTimeMs Timestamp of the FIRST sample in the batch, ms.
     *                     Subsequent samples are assumed equidistant at
     *                     sampleRateHz.
     * @return true  if at least one gesture was detected in this batch.
     * @return false otherwise.
     */
    bool addBatch(const ImuSample* samples,
                  size_t           count,
                  uint32_t         baseTimeMs) noexcept;

    /*--------------------------------------------------------------------------
     * Diagnostics
     *------------------------------------------------------------------------*/

    /** @brief Total gesture events fired since construction or last reset(). */
    uint32_t totalEvents()    const noexcept { return mTotalEvents; }

    /** @brief Remaining cooldown in samples (0 = ready to trigger). */
    uint32_t cooldownRemaining() const noexcept { return mCooldownRemaining; }

    /** @brief Current GX integral value (degrees) for the last sample. */
    float    lastGxIntegral() const noexcept { return mGxIntegralCache; }

private:
    /*--------------------------------------------------------------------------
     * Internal helpers
     *------------------------------------------------------------------------*/
    void  pushSample(const ImuSample& s) noexcept;
    float computeGxIntegral()  const noexcept;
    bool  evaluateSample(float azG,
                         float gxIntegralDeg,
                         uint32_t timestampMs) noexcept;

    /*--------------------------------------------------------------------------
     * Storage
     *------------------------------------------------------------------------*/
    static constexpr uint8_t MAX_PRE_WIN = 64u;

    Config      mCfg;
    IListener*  mListener             = nullptr;

    /** Ring buffer: az and gx for the last preWinSamples. */
    float       mAzBuf[MAX_PRE_WIN]  = {};
    float       mGxBuf[MAX_PRE_WIN]  = {};
    uint8_t     mHead                 = 0u;   /**< Write position in ring buf */
    uint8_t     mFilled               = 0u;   /**< Samples actually written   */

    /** Running GX integral (degrees). Updated incrementally O(1). */
    float       mGxSum               = 0.0f;
    float       mGxIntegralCache    = 0.0f;
    float       mDt                   = 0.02f; /**< 1/sampleRateHz           */

    uint32_t    mCooldownRemaining   = 0u;
    uint32_t    mCooldownSamples     = 0u;    /**< cooldownS × sample_rate   */
    uint32_t    mTotalEvents         = 0u;
};
