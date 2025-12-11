
#pragma once

#include "SDK/Messages/MessageBase.hpp"
#include "SDK/Messages/MessageTypes.hpp"
#include "SDK/Messages/CommandMessages.hpp"

#include "AppTypes.hpp"
#include <vector>

namespace CustomMessage {

    // Application custom commands

    // Service <-> GUI
    constexpr SDK::MessageType::Type ALARM_LIST             = 0x00000001;

    // Service --> GUI
    constexpr SDK::MessageType::Type ACTIVATED_ALARM        = 0x00000002;

    // GUI --> Service
    constexpr SDK::MessageType::Type ACTIVATED_EFFECT       = 0x00000003;
    constexpr SDK::MessageType::Type ALARM_STOP             = 0x00000004;
    constexpr SDK::MessageType::Type ALARM_STOP_ALL         = 0x00000005;
    constexpr SDK::MessageType::Type ALARM_SNOOZE           = 0x00000006;
    constexpr SDK::MessageType::Type ALARM_SNOOZE_ALL       = 0x00000007;



    // Updated alarm list
    struct AlarmList : public SDK::MessageBase {
        std::vector<AppType::Alarm> list {};
        AlarmList()
            : SDK::MessageBase(ALARM_LIST)
        {}
    };

    struct ActivatedAlarm : public SDK::MessageBase {
        AppType::Alarm alarm {};
        ActivatedAlarm()
            : SDK::MessageBase(ACTIVATED_ALARM)
        {}
    };

    struct AlarmActiveteEffect : public SDK::MessageBase {
        AppType::Alarm alarm {};
        AlarmActiveteEffect()
            : SDK::MessageBase(ACTIVATED_EFFECT)
        {}
    };

    struct AlarmStop : public SDK::MessageBase {
        AppType::Alarm alarm {};
        AlarmStop()
            : SDK::MessageBase(ALARM_STOP)
        {}
    };

    struct AlarmStopAll : public SDK::MessageBase {
        AlarmStopAll()
            : SDK::MessageBase(ALARM_STOP_ALL)
        {}
    };

    struct AlarmSnooze : public SDK::MessageBase {
        AppType::Alarm alarm {};
        AlarmSnooze()
            : SDK::MessageBase(ALARM_SNOOZE)
        {}
    };

    struct AlarmSnoozeAll : public SDK::MessageBase {
        AlarmSnoozeAll()
            : SDK::MessageBase(ALARM_SNOOZE_ALL)
        {}
    };
}

