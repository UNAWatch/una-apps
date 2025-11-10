/**
 ******************************************************************************
 * @file    ActivitySummarySerializer.hpp
 * @date    08-04-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Serializes/Deserializes summary data to a file.
 ******************************************************************************
 *
 * The serialized format follows this structure:
 *
 * @code{.json}
 * {
 *     "file":"Activity/202504/activity_20250409T023510.cbor",
 *     "date":"20250409T023530",
 *     "time":125,
 *     "distance":4.0154,
 *     "speed_avg":3.64264,
 *     "elevation":98.4013,
 *     "pace_avg":192,
 *     "hr_max":84,
 *     "hr_avg":74,
 *     "map":"416A426B43...6643"
 * }
 * @endcode
 *
 ******************************************************************************
 */

#ifndef __ACTIVITY_SUMMARY_SERIALIZER_HPP
#define __ACTIVITY_SUMMARY_SERIALIZER_HPP

#include "SDK/Kernel/Kernel.hpp"

#include "ActivitySummary.hpp"

 /**
  * @class ActivitySummarySerializer
  * @brief Serializes/Deserializes summary data to a file.
  */
class ActivitySummarySerializer {

public:

    /**
     * @brief Constructor.
     * @param kernel: Reference to the kernel interface.
     * @param pathToFile: Path to the summary file.
     */
    ActivitySummarySerializer(const SDK::Kernel& kernel, const char* pathToFile);

    /**
     * @brief Destructor.
     */
    virtual ~ActivitySummarySerializer() = default;

    /**
     * @brief Save summary.
     * @param summary: Reference to the Summary structure.
     * @return True if summary saved successfully.
     */
    bool save(const ActivitySummary& summary);

    /**
     * @brief Save summary.
     * @param summary: Reference to load the Summary structure.
     * @return True if summary read successfully.
     */
    bool load(ActivitySummary& summary);

private:
    /// A constant reference to an Kernel object.
    const SDK::Kernel& mKernel;

    /// Path to summary file
    const char* mPath = nullptr;
};

#endif /* __ACTIVITY_SUMMARY_SERIALIZER_HPP */
