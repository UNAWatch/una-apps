/**
 ******************************************************************************
 * @file    ActivityWriter.hpp
 * @date    08-04-2025
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Serializes activity data to a file.
 ******************************************************************************
 *
 * The serialized format follows this structure:
 *
 * @code{.json}
 * {
 *   "type":"running", // activity type (e.g. 'running', 'cycling', 'hiking’)
 *   "start_time":"20250202T204737", // local start time of the activity in ISO 8601 format
 *   "lap":[ // array of laps
 *     { // first lap
 *       "st":10, // lap start time offset from the start of session activity ("start_time") in seconds
 *       "data":[ // array of activity data. The size of the objects in this field has the greatest impact on the size of the output file, so the key names are abbreviated.
 *         [        // first data sample
 *           0,     // sample time offset from the start of session activity ("start_time") in seconds
 *           75,    // heart rate in bpm
 *           50450168,  // latitude x1000000
 *           30523608,  // longitude x1000000
 *           99         // altitude in meters
 *         ],
 *         [ // second data sample
 *           20,
 *           110,
 *           52229770,
 *           21011779,
 *           100
 *         ]
 *       ],
 *       "et":300 // lap start time offset from the start of session activity ("start_time") in seconds
 *     },
 *     { // second lap
 *       "st":310,
 *       "data":[
 *         [
 *           310,
 *           110,
 *           52229780,
 *           21011779,
 *           100
 *         ],
 *         [
 *           320,
 *           110,
 *           52229630,
 *           21011219,
 *           100
 *         ]
 *       ],
 *       "et":600
 *     }
 *   ],
 *   "end_time_offset":600 // activity end time offset from the start of session activity ("start_time") in seconds
 * }
 * @endcode
 * 
 * Call sequence:
 *      startSession()
 *       startLaps()
 *        startLap()
 *         addDataPoints()
 *        endLap()
 *        startLap()
 *         addDataPoints()
 *        endLap()
 *       endLaps()
 *      endSession()
 *  optional: 
 *      getFilePath()
 *      deleteSession()
 *
 ******************************************************************************
 */

#ifndef __ACTIVITY_WRITER_HPP
#define __ACTIVITY_WRITER_HPP

#include <cstdint>
#include <cstdbool>

#include "IFileSystem.hpp"
#include "CborStreamWriter.hpp"

/**
 * @class ActivityWriter
 * @brief Serializes activity data to a file.
 */
class ActivityWriter {

public:

    /**
     * @struct DataPoint
     * @brief Represents a single activity data point.
     */
    struct DataPoint {
        uint16_t timeOffset; ///< Sample timestamp in seconds from session start.
        uint8_t heartRate;   ///< Heart rate in beats per minute.
        struct {
            float latitude;  ///< Latitude coordinate.
            float longitude; ///< Longitude coordinate.
            float altitude;  ///< Altitude in meters.
        } location;
    };

    /**
     * @brief Constructor.
     * @param fs: Reference to the file system interface.
     * @param pathToDir: Base directory path for storing activity files.
     *        The path must be end with '/'
     */
    ActivityWriter(Interface::IFileSystem &fs, const char *pathToDir);

    /**
     * @brief Destructor.
     */
    virtual ~ActivityWriter() = default;

    /**
     * @brief Starts a new activity session.
     * @param activityType: Activity type (e.g., "running").
     * @param timestamp: Session start timestamp.
     * @return True if session initialization was successful.
     */
    bool startSession(const char *activityType, std::tm timestamp);

    /**
     * @brief Ends the current session.
     * @param endTimeOffset: Session end time offset in seconds.
     * @return True if session ended successfully.
     */
    bool endSession(uint16_t endTimeOffset);

    /**
     * @brief Delete current session.
     * @return True if session deleted successfully.
     */
    bool deleteSession();

    /**
     * @brief Returns the file's path.
     * @retval Pointer to the path string.
     */
    const char* getFilePath() const;

    /**
     * @brief Starts lap array.
     * @return True if lap started successfully.
     */
    bool startLaps();

    /**
     * @brief End lap array.
     * @return True if lap started successfully.
     */
    bool endLaps();

    /**
     * @brief Starts a new lap within the session.
     * @param startTimeOffset: Lap start time offset in seconds.
     * @return True if lap started successfully.
     */
    bool startLap(uint16_t startTimeOffset);

    /**
     * @brief Adds multiple data points to the current lap.
     * @param pDataPoints: Pointer to an array of data points.
     * @param count: Number of data points in the array.
     * @return True if all data points were successfully added.
     */
    bool addDataPoints(const DataPoint *pDataPoints, size_t count);

    /**
     * @brief Ends the current lap.
     * @param endTimeOffset: Lap end time offset in seconds.
     * @return True if lap ended successfully.
     */
    bool endLap(uint16_t endTimeOffset);

private:
    Interface::IFileSystem &mFs;        ///< File system interface reference.
    const char *mBasePath = nullptr;    ///< Base directory path.

    std::unique_ptr<Interface::IFile> mFile; ///< File handle for writing data.
    bool mIsActive = false;             ///< Current session is active.

    CborStreamWriter mWriter;           ///< Stream writer to CBOR file

    bool createFileAndOpen(std::tm timestamp);
    void closeFile();
};

#endif /* __ACTIVITY_WRITER_HPP */
