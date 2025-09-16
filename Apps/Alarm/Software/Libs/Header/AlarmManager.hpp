/**
 ******************************************************************************
 * @file    AlarmManager.hpp
 * @date    08-11-2024
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Alarms manager class for handling alarm functionality.
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef __ALARM_MANAGER_HPP
#define __ALARM_MANAGER_HPP

#include <cstdint>
#include <cstdbool>
#include <array>
#include <vector>
#include <memory>
#include <string_view>

#include "SDK/Interfaces/IKernel.hpp"
#include "SDK/JSON/JsonStreamReader.hpp"
#include "SDK/JSON/JsonStreamWriter.hpp"

#include "AppTypes.hpp"

 /**
  * @class AlarmManager
  * @brief Manages and processes alarms.
  */
class AlarmManager {

public:

    /**
     * @class AlarmCallback
     * @brief Callback interface for handling alarm events.
     */
    class AlarmCallback {
    public:
        /**
         * @brief Callback triggered when an alarm activates.
         * @param alarm: The active alarm information.
         */
        virtual void onAlarm(const AppType::Alarm& alarm) {}

        /**
         * @brief Callback triggered when the alarm list changes.
         * @param list: The updated list of alarms.
         */
        virtual void onListChanged(const std::vector<AppType::Alarm>& list) {}
    protected:
        virtual ~AlarmCallback() = default;
    };

    /**
     * @brief Constructor for AlarmManager.
     * @param kernel: Reference to the kernel interface.
     */
    AlarmManager(const IKernel& kernel);

    /**
     * @brief Destructor for AlarmManager.
     */
    virtual ~AlarmManager();

    /**
     * @brief Load the alarms from persistent storage.
     */
    void load();

    /**
     * @brief Attach a callback to receive alarm events.
     * @param pCallback: Pointer to the callback interface.
     */
    void attachCallback(AlarmCallback* pCallback)
    {
        mObserver = pCallback;
    }

    /**
     * @brief Timer callback function to be called periodically (every minute).
     * This function checks for alarms that need to be triggered based on the current time.
     * @param tmNow: The current time as a std::tm structure.
     * @return Time in milliseconds until the next required call. Can be called earlier but not later than this time.
     */
    uint32_t execute(const std::tm& tmNow);

    /**
     * @brief Retrieve the current alarm list.
     * @return The current list of alarms.
     */
    const std::vector<AppType::Alarm>& getAlarmList();

    /**
     * @brief Save a new alarm list to persistent storage.
     * @param list: The list of alarms to save.
     * @return 'true' if the save was successful, 'false' otherwise.
     */
    bool saveAlarmList(const std::vector<AppType::Alarm>& list);

    /**
     * @brief Disable a specific alarm.
     * @param alarm: The alarm to disable.
     */
    void disableAlarm(const AppType::Alarm& alarm);

    /**
     * @brief Disable all currently active alarms.
     */
    void disableAllActiveAlarm();

    /**
     * @brief Snooze a specific alarm, delaying it by a set interval.
     * @param alarm: The alarm to snooze.
     */
    void snoozeAlarm(const AppType::Alarm& alarm);

    /**
     * @brief Snooze all currently active alarms.
     */
    void snoozeAllActiveAlarm();

    /**
     * @brief Check if there are any active alarms or snoozed alarms.
     * @return true if there are active alarms that need monitoring, false otherwise.
     */
    bool hasActiveAlarms() const;

private:

    /// A constant reference to an IKernel object.
    const IKernel& mKernel;

    ///  File path for storing alarms.
    static constexpr char skFilePath[] = "alarms.json";

    /// Structure storing all alarms
    std::vector<AppType::Alarm> mAlarms{};

    /// Buffer for JSON operations.
    char mBuffer[1024]{};

    /// Observer for callbacks.
    AlarmCallback* mObserver = nullptr;

    /// JSON values for 'repeat' field
    inline static constexpr std::array<std::string_view, AppType::Alarm::REPEAT_COUNT> kRepeatJsonKeyValue =
    { "no", "every_day", "week_days", "weekends",
        "monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday" };

    /// JSON values for 'effect' field  
    inline static constexpr std::array<std::string_view, AppType::Alarm::EFFECT_COUNT> kEffectJsonKeyValue =
    { "beep_vibro", "vibro", "beep" };

    /// Time interval for snoozed alarms in minutes.
    static constexpr uint8_t kSnoozedTimeMinutes = 1;

    /// Maximum number of snoozes.
    static constexpr uint8_t kMaxSnoozeCount = 3;

    /// Initial list size.
    static constexpr size_t kInitialCount = 20;

    /**
     * @struct SnoozedAlarm
     * @brief Structure for storing snoozed alarm details.
     */
    struct SnoozedAlarm {
        AppType::Alarm info;                    ///< Original alarm info.
        uint8_t snoozeCount = kMaxSnoozeCount;  ///< Count of remaining snoozes.
        uint8_t nextTriggerHour = 0;            ///< Next hour for the alarm trigger.
        uint8_t nextTriggerMinute = 0;          ///< Next minute for the alarm trigger.
    };

    /// List of snoozed alarms.
    std::vector<SnoozedAlarm> mSnoozedAlarms;

    /**
     * @brief Save alarms to the file.
     * @param alarms: List of alarms to save.
     * @return 'true' if saving was successful, 'false' otherwise.
     */
    bool saveTofile(const std::vector<AppType::Alarm>& alarms);

    /**
     * @brief Load alarms from the file.
     * @param alarms: Structure to store the loaded alarms.
     * @return 'true' if loading was successful, 'false' otherwise.
     */
    bool loadFromFile(std::vector<AppType::Alarm>& alarms);

    /**
     * @brief Create JSON data from alarms.
     * @param alarms: List of alarms to serialize.
     * @param buff: Buffer to store JSON data.
     * @param buffSize: Size of the buffer.
     * @return Number of bytes written to buffer.
     */
    uint32_t createJSON(const std::vector<AppType::Alarm>& alarms, char* buff, uint32_t buffSize);

    /**
     * @brief Parse JSON data and load it into alarms.
     * @param buff: Buffer containing JSON data.
     * @param length: Length of the buffer.
     * @param alarms: Structure to store parsed alarms.
     * @return 'true' if parsing was successful, 'false' otherwise.
     */
    bool parseJSON(char* buff, uint32_t length, std::vector<AppType::Alarm>& alarms);

    /**
     * @brief Print all alarms to log for debugging.
     * @param alarms: List of alarms to print.
     */
    void dump(const std::vector<AppType::Alarm>& alarms);

    /**
     * @brief Check if any alarms are due at the current time.
     * @param currentHour: Current hour.
     * @param currentMinute: Current minute.
     * @param currentDay: Current day of the week.
     * @param tmNow: Current time structure for snooze calculations.
     */
    void checkAlarms(uint8_t currentHour, uint8_t currentMinute, uint8_t currentDay, const std::tm& tmNow);

    /**
     * @brief Add a snoozed alarm.
     * @param alarm: The alarm to snooze.
     * @param tmNow: Current time structure for calculations.
     */
    void addSnoozedAlarm(const AppType::Alarm& alarm, const std::tm& tmNow);

    /**
     * @brief Update trigger time for a snoozed alarm.
     * @param snoozed: Snoozed alarm to update.
     * @param tmNow: Current time structure for calculations.
     */
    void updateSnoozedTriggerTime(SnoozedAlarm& snoozed, const std::tm& tmNow);

    /**
     * @brief Remove snoozed alarms that are no longer active.
     */
    void removeObsoleteSnoozedAlarms();

    /**
     * @brief Check if an alarm is due on the current day.
     * @param alarm: Alarm to check.
     * @param currentDay: Current day of the week.
     * @return 'true' if the alarm is due today, 'false' otherwise.
     */
    bool isAlarmDueToday(const AppType::Alarm& alarm, uint8_t currentDay) const;

    /**
     * @brief Check if an alarm is currently snoozed.
     * @param alarm: Alarm to check.
     * @return 'true' if the alarm is snoozed, 'false' otherwise.
     */
    bool isSnoozed(const AppType::Alarm& alarm) const;

    /**
     * @brief Disable a specific one-time alarm in mAlarms.
     * @param alarm: The one-time alarm to disable.
     * @return true if alarm was disabled and save is needed, false otherwise.
     */
    bool disableOneTimeAlarm(const AppType::Alarm& alarm);
};

#endif /* __ALARM_MANAGER_HPP */
