/**
 ******************************************************************************
 * @file    AlarmManager.cpp
 * @date    08-11-2024
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Alarms manager class for handling alarm functionality.
 ******************************************************************************
 *
 ******************************************************************************
 */

#include "AlarmManager.hpp"

#define TAG "App::Alarm[S]::AlarmManager"
#define LOG_MODULE_PRX      TAG"::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "Logger.h"

#include <algorithm>
#include "JsonStreamReader.hpp"
#include "JsonStreamWriter.hpp"

AlarmManager::AlarmManager(const IKernel& kernel) : mKernel(kernel),
mObserver()
{
    mAlarms.reserve(kInitialCount);
    mSnoozedAlarms.reserve(kInitialCount / 4);
}

AlarmManager::~AlarmManager()
{}

void AlarmManager::load()
{
    loadFromFile(mAlarms);

    LOG_DEBUG("Alarms loaded\n");

    if (mObserver) {
        mObserver->onListChanged(mAlarms);
    }

#if 1
    dump(mAlarms);
#endif
}

uint32_t AlarmManager::execute(const std::tm& tmNow)
{
    checkAlarms(static_cast<uint8_t>(tmNow.tm_hour),
        static_cast<uint8_t>(tmNow.tm_min),
        static_cast<uint8_t>(tmNow.tm_wday),
        tmNow);

    // Calculate time until next minute (when next alarms can trigger)
    uint32_t nextCheckMs = (60 - tmNow.tm_sec) * 1000;

    LOG_DEBUG("Next alarm check in %u ms\n", nextCheckMs);
    return nextCheckMs;
}

const std::vector<AppType::Alarm>& AlarmManager::getAlarmList()
{
    return mAlarms;
}

bool AlarmManager::saveAlarmList(const std::vector<AppType::Alarm>& list)
{
    bool status = saveTofile(list);

    if (status) {
        mAlarms = list;

        // Remove obsolete snoozed alarms when alarm list changes
        removeObsoleteSnoozedAlarms();

        LOG_DEBUG("Alarms saved\n");

        if (mObserver) {
            mObserver->onListChanged(mAlarms);
        }
    }

    return status;
}

void AlarmManager::disableAlarm(const AppType::Alarm& alarm)
{
    // Find and remove from snoozed list
    auto it = std::find_if(mSnoozedAlarms.begin(), mSnoozedAlarms.end(),
        [&](const SnoozedAlarm& snoozed) {
            return snoozed.info == alarm;
        });

    if (it != mSnoozedAlarms.end()) {
        LOG_DEBUG("Removing snoozed alarm: %02d:%02d\n",
            it->info.timeHours, it->info.timeMinutes);

        mSnoozedAlarms.erase(it);
        // Note: One-time alarms are already disabled when first triggered
        // No need to disable them again
    }

    // Clean up obsolete snoozed alarms
    removeObsoleteSnoozedAlarms();
}

void AlarmManager::disableAllActiveAlarm()
{
    if (mSnoozedAlarms.empty()) {
        return;
    }

    // Log removal of all snoozed alarms
    for (const auto& snoozed : mSnoozedAlarms) {
        LOG_DEBUG("Removing snoozed alarm: %02d:%02d\n",
            snoozed.info.timeHours, snoozed.info.timeMinutes);
    }

    // Clear all snoozed alarms
    mSnoozedAlarms.clear();
    // Note: One-time alarms are already disabled when first triggered
    // No need to disable them again
}

void AlarmManager::snoozeAlarm(const AppType::Alarm& alarm)
{
    auto it = std::find_if(mSnoozedAlarms.begin(), mSnoozedAlarms.end(),
        [&](const SnoozedAlarm& snoozed) {
            return snoozed.info == alarm;
        });

    if (it != mSnoozedAlarms.end()) {
        // Alarm is already snoozed, no additional action needed
        LOG_DEBUG("Alarm already snoozed: %02d:%02d\n",
            it->info.timeHours, it->info.timeMinutes);
    }
}

void AlarmManager::snoozeAllActiveAlarm()
{
    for (const auto& snoozed : mSnoozedAlarms) {
        // All alarms are already snoozed, no additional action needed
        LOG_DEBUG("Alarm already snoozed: %02d:%02d\n",
            snoozed.info.timeHours, snoozed.info.timeMinutes);
    }
}

bool AlarmManager::hasActiveAlarms() const
{
    bool hasEnabledAlarms = std::any_of(mAlarms.begin(), mAlarms.end(),
        [](const AppType::Alarm& alarm) {
            return alarm.on;
        });

    bool hasSnoozedAlarms = !mSnoozedAlarms.empty();

    return hasEnabledAlarms || hasSnoozedAlarms;
}

bool AlarmManager::saveTofile(const std::vector<AppType::Alarm>& alarms)
{
    bool rv = false;
    size_t bw = 0;

    // Create file object
    auto file = mKernel.fs.file(skFilePath);
    if (!file) {
        LOG_ERROR("Failed to create file object for %s\n", skFilePath);
        return false;
    }

    // Generate JSON
    size_t len = createJSON(alarms, mBuffer, sizeof(mBuffer));
    if (len > 0) {
        if (file->open(true, true)) {
            if (file->write(mBuffer, len, bw) && bw == len) {
                rv = true;
            }
            file->close();
        }
    }

    if (!rv) {
        LOG_ERROR("Failed to save alarms!\n");
    }
    return rv;
}

bool AlarmManager::loadFromFile(std::vector<AppType::Alarm>& alarms)
{
    bool rv = false;
    size_t br = 0;

    // Create file object
    auto file = mKernel.fs.file(skFilePath);
    if (!file) {
        LOG_ERROR("Failed to create file object for %s\n", skFilePath);
        return false;
    }

    size_t len = file->size();
    if (len < sizeof(mBuffer)) {
        if (file->open()) {
            if (file->read(mBuffer, len, br) && br > 0) {
                rv = parseJSON(mBuffer, len, alarms);
            }
            file->close();
        }
    } else {
        LOG_ERROR("Buffer is too small. Required %u bytes\n", len);
    }

    if (!rv) {
        LOG_ERROR("Can't read alarms file or file is corrupted.\n");
    }

    return rv;
}

bool AlarmManager::parseJSON(char* buff, uint32_t length, std::vector<AppType::Alarm>& alarms)
{
    sdk::JsonStreamReader reader{ buff, length };
    if (!reader.validate()) {
        LOG_ERROR("JSON is invalid\n");
        return false;
    }

    // Clear the alarms vector before parsing
    alarms.clear();

    // Get array length
    size_t arrayLength = 0;
    if (!reader.getArrayLength("alarms", arrayLength)) {
        LOG_ERROR("Failed to get alarms array length\n");
        return false;
    }

    // Reserve space for efficiency
    alarms.reserve(arrayLength);

    // Parse each alarm in the array
    for (size_t i = 0; i < arrayLength; i++) {
        AppType::Alarm alarm{};
        char query[32];

        // Parse 'on' field
        snprintf(query, sizeof(query), "alarms[%u].on", static_cast<unsigned>(i));
        if (!reader.get(query, alarm.on)) {
            LOG_ERROR("Failed to parse 'on' field for alarm %u\n", static_cast<unsigned>(i));
            continue;
        }

        // Parse 'time_h' field
        snprintf(query, sizeof(query), "alarms[%u].time_h", static_cast<unsigned>(i));
        if (!reader.get(query, alarm.timeHours) || alarm.timeHours >= 24) {
            LOG_ERROR("Failed to parse or invalid 'time_h' field for alarm %u\n", static_cast<unsigned>(i));
            continue;
        }

        // Parse 'time_m' field
        snprintf(query, sizeof(query), "alarms[%u].time_m", static_cast<unsigned>(i));
        if (!reader.get(query, alarm.timeMinutes) || alarm.timeMinutes >= 60) {
            LOG_ERROR("Failed to parse or invalid 'time_m' field for alarm %u\n", static_cast<unsigned>(i));
            continue;
        }

        // Parse 'repeat' field
        snprintf(query, sizeof(query), "alarms[%u].repeat", static_cast<unsigned>(i));
        std::string_view repeatStr;
        if (!reader.get(query, repeatStr)) {
            LOG_ERROR("Failed to parse 'repeat' field for alarm %u\n", static_cast<unsigned>(i));
            continue;
        }

        // Find matching repeat value
        bool repeatFound = false;
        for (uint8_t j = 0; j < AppType::Alarm::REPEAT_COUNT; j++) {
            if (kRepeatJsonKeyValue[j] == repeatStr) {
                alarm.repeat = static_cast<AppType::Alarm::Repeat>(j);
                repeatFound = true;
                break;
            }
        }
        if (!repeatFound) {
            LOG_ERROR("Invalid 'repeat' value for alarm %u: %.*s\n",
                static_cast<unsigned>(i), static_cast<int>(repeatStr.length()), repeatStr.data());
            continue;
        }

        // Parse 'effect' field
        snprintf(query, sizeof(query), "alarms[%u].effect", static_cast<unsigned>(i));
        std::string_view effectStr;
        if (!reader.get(query, effectStr)) {
            LOG_ERROR("Failed to parse 'effect' field for alarm %u\n", static_cast<unsigned>(i));
            continue;
        }

        // Find matching effect value
        bool effectFound = false;
        for (uint8_t j = 0; j < AppType::Alarm::EFFECT_COUNT; j++) {
            if (kEffectJsonKeyValue[j] == effectStr) {
                alarm.effect = static_cast<AppType::Alarm::Effect>(j);
                effectFound = true;
                break;
            }
        }
        if (!effectFound) {
            LOG_ERROR("Invalid 'effect' value for alarm %u: %.*s\n",
                static_cast<unsigned>(i), static_cast<int>(effectStr.length()), effectStr.data());
            continue;
        }

        // Add valid alarm to the vector
        alarms.push_back(alarm);
    }

    LOG_DEBUG("Successfully parsed %u alarms\n", static_cast<unsigned>(alarms.size()));
    return true;
}

uint32_t AlarmManager::createJSON(const std::vector<AppType::Alarm>& alarms, char* buff, uint32_t buffSize)
{
    sdk::JsonStreamWriter writer{ buff, buffSize };

    // Start root object
    writer.startMap();

    // Start "alarms" array
    {
        sdk::JsonStreamWriter::KeyedArrayScope alarmsArray{ writer, "alarms", alarms.size() };

        // Add each alarm to the array
        for (const auto& alarm : alarms) {
            sdk::JsonStreamWriter::MapScope alarmObj{ writer };

            writer.add("on", alarm.on);
            writer.add("time_h", alarm.timeHours);
            writer.add("time_m", alarm.timeMinutes);

            // Add repeat field with string value
            if (alarm.repeat < AppType::Alarm::REPEAT_COUNT) {
                writer.add("repeat", kRepeatJsonKeyValue[alarm.repeat].data());
            } else {
                LOG_ERROR("Invalid repeat value: %u\n", static_cast<unsigned>(alarm.repeat));
                writer.add("repeat", "no");  // Default fallback
            }

            // Add effect field with string value
            if (alarm.effect < AppType::Alarm::EFFECT_COUNT) {
                writer.add("effect", kEffectJsonKeyValue[alarm.effect].data());
            } else {
                LOG_ERROR("Invalid effect value: %u\n", static_cast<unsigned>(alarm.effect));
                writer.add("effect", "beep_vibro");  // Default fallback
            }
        }
    }

    // End root object
    writer.endMap();

    // Check for errors
    if (writer.isError()) {
        LOG_ERROR("Failed to create JSON\n");
        buff[0] = '\0';  // Clear buffer on error
        return 0;
    }

    // Return the length of the created JSON string
    uint32_t jsonLength = static_cast<uint32_t>(strlen(buff));
    LOG_DEBUG("Successfully created JSON with %u bytes\n", jsonLength);
    return jsonLength;
}

void AlarmManager::dump(const std::vector<AppType::Alarm>& alarms)
{
    for (size_t i = 0; i < alarms.size(); i++) {
        LOG_DEBUG("alarm %d: on %d, %02d:%02d, repeat %d, effect %d\n", i,
            alarms[i].on, alarms[i].timeHours,
            alarms[i].timeMinutes, alarms[i].repeat, alarms[i].effect);
    }
}

void AlarmManager::checkAlarms(uint8_t currentHour, uint8_t currentMinute, uint8_t currentDay, const std::tm& tmNow)
{
    bool needSave = false;

    // Check active alarms from mAlarms (only enabled ones)
    for (auto& alarm : mAlarms) {
        if (!alarm.on) continue;  // Skip disabled alarms

        if (alarm.timeHours == currentHour
            && alarm.timeMinutes == currentMinute
            && isAlarmDueToday(alarm, currentDay)) {
            if (!isSnoozed(alarm)) {
                LOG_DEBUG("Triggering new alarm: %02d:%02d\n", alarm.timeHours, alarm.timeMinutes);
                if (mObserver) {
                    mObserver->onAlarm(alarm);
                }
                addSnoozedAlarm(alarm, tmNow);

                // Immediately disable one-time alarms after triggering
                if (alarm.repeat == AppType::Alarm::REPEAT_NO) {
                    // Find and disable the alarm in mAlarms
                    auto alarmIt = std::find(mAlarms.begin(), mAlarms.end(), alarm);
                    if (alarmIt != mAlarms.end() && alarmIt->on) {
                        alarmIt->on = false;
                        needSave = true;
                        LOG_DEBUG("Immediately disabled one-time alarm: %02d:%02d\n",
                            alarm.timeHours, alarm.timeMinutes);
                    }
                }
            }
        }
    }

    // Check snoozed alarms and remove completed ones
    auto it = mSnoozedAlarms.begin();
    while (it != mSnoozedAlarms.end()) {
        if (it->snoozeCount > 0 &&
            it->nextTriggerHour == currentHour &&
            it->nextTriggerMinute == currentMinute) {
            it->snoozeCount--;

            if (it->snoozeCount > 0) {
                LOG_DEBUG("Triggering snoozed alarm: %02d:%02d\n",
                    it->info.timeHours, it->info.timeMinutes);
                if (mObserver) {
                    mObserver->onAlarm(it->info);
                }
                updateSnoozedTriggerTime(*it, tmNow);
                ++it;
            } else {
                // Snooze exhausted - remove from snoozed list
                LOG_DEBUG("Snooze exhausted, removing alarm: %02d:%02d\n",
                    it->info.timeHours, it->info.timeMinutes);

                it = mSnoozedAlarms.erase(it);  // Remove from snoozed list
                // Note: One-time alarms are already disabled when first triggered
            }
        } else {
            ++it;
        }
    }

    // Save only once at the end if there were changes
    if (needSave) {
        saveTofile(mAlarms);
        if (mObserver) {
            mObserver->onListChanged(mAlarms);
        }
    }
}

void AlarmManager::addSnoozedAlarm(const AppType::Alarm& alarm, const std::tm& tmNow)
{
    SnoozedAlarm snoozed;
    snoozed.info = alarm;
    snoozed.snoozeCount = kMaxSnoozeCount;
    updateSnoozedTriggerTime(snoozed, tmNow);

    mSnoozedAlarms.push_back(std::move(snoozed));
}

void AlarmManager::updateSnoozedTriggerTime(SnoozedAlarm& snoozed, const std::tm& tmNow)
{
    // Calculate next trigger time by adding snooze minutes to current time
    uint16_t totalMinutes = static_cast<uint16_t>(tmNow.tm_hour * 60 + tmNow.tm_min + kSnoozedTimeMinutes);

    // Handle day overflow (wrap around at 24:00)
    if (totalMinutes >= 24 * 60) {
        totalMinutes -= 24 * 60;
    }

    snoozed.nextTriggerHour = static_cast<uint8_t>(totalMinutes / 60);
    snoozed.nextTriggerMinute = static_cast<uint8_t>(totalMinutes % 60);

    LOG_DEBUG("Next snooze trigger set to %02d:%02d\n",
        snoozed.nextTriggerHour, snoozed.nextTriggerMinute);
}

void AlarmManager::removeObsoleteSnoozedAlarms()
{
    mSnoozedAlarms.erase(
        std::remove_if(mSnoozedAlarms.begin(), mSnoozedAlarms.end(),
            [this](const SnoozedAlarm& snoozed) {
                // Find corresponding alarm in mAlarms
                auto it = std::find(mAlarms.begin(), mAlarms.end(), snoozed.info);
                // Remove if alarm doesn't exist or is disabled
                return it == mAlarms.end() || !it->on;
            }),
        mSnoozedAlarms.end());
}

bool AlarmManager::isAlarmDueToday(const AppType::Alarm& alarm, uint8_t currentDay) const
{
    switch (alarm.repeat) {
    case AppType::Alarm::REPEAT_NO:
        return true;
    case AppType::Alarm::REPEAT_EVERY_DAY:
        return true;
    case AppType::Alarm::REPEAT_WEEK_DAYS:
        return currentDay >= 1 && currentDay <= 5;
    case AppType::Alarm::REPEAT_WEEKENDS:
        return currentDay == 0 || currentDay == 6;
    case AppType::Alarm::REPEAT_MONDAY:
        return currentDay == 1;
    case AppType::Alarm::REPEAT_TUESDAY:
        return currentDay == 2;
    case AppType::Alarm::REPEAT_WEDNESDAY:
        return currentDay == 3;
    case AppType::Alarm::REPEAT_THURSDAY:
        return currentDay == 4;
    case AppType::Alarm::REPEAT_FRIDAY:
        return currentDay == 5;
    case AppType::Alarm::REPEAT_SATURDAY:
        return currentDay == 6;
    case AppType::Alarm::REPEAT_SUNDAY:
        return currentDay == 0;
    default:
        return false;
    }
}

bool AlarmManager::isSnoozed(const AppType::Alarm& alarm) const
{
    return std::any_of(mSnoozedAlarms.begin(), mSnoozedAlarms.end(),
        [&](const SnoozedAlarm& snoozed) {
            return snoozed.info == alarm;
        });
}

bool AlarmManager::disableOneTimeAlarm(const AppType::Alarm& alarm)
{
    // Find and disable one-time alarm in mAlarms
    auto it = std::find(mAlarms.begin(), mAlarms.end(), alarm);
    if (it != mAlarms.end() && it->on && it->repeat == AppType::Alarm::REPEAT_NO) {
        it->on = false;

        LOG_DEBUG("Auto-disabled one-time alarm: %02d:%02d\n",
            it->timeHours, it->timeMinutes);

        return true;  // Indicate that save is needed
    }

    return false;  // No changes made
}
