/**
 ******************************************************************************
 * @file    AppTypes.hpp
 * @date    24-07-2024
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Common application types.
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef __APP_TYPES_HPP
#define __APP_TYPES_HPP

#include <cstdint>
#include <cstdbool>
#include <cstring>
#include <variant>
#include <memory>
#include <ctime>

#if !defined(SIMULATOR)
#include "main.h"
#else // defined(SIMULATOR)

#endif // defined(SIMULATOR)

/**
 * @namespace AppType
 * @brief Namespace for application-specific types.
 */
namespace AppType
{

constexpr uint8_t kSerialNumberLen = 12;

struct DeviceInfo {
    const char *name = "";
    const char *manufacturer = "";
    const char *id = "";
    const char *serialNumber = "";
    const char *swVersion = "";
    const char *hwVersion = "";
    const char *btVersion = "";
    const char *fccId = "";
    const char *ce = "";
};


// Data types to communicate GUI <-> Backend
namespace G2BEvent
{
    struct Shutdown {};
    struct VibroShort {};
    struct VibroLong {};
    struct BuzzShort {};
    struct BuzzLong {};
    struct Backlight {
        bool status;
    };

    using Data = std::variant<
            Shutdown,
            VibroShort,
            VibroLong,
            BuzzShort,
            BuzzLong,
            Backlight
            >;
};

namespace B2GEvent {
    struct Booted {};

    struct DeviceInfo {
        AppType::DeviceInfo info;
    };

    struct BatteryLevel {
        float level;
    };
    struct BatteryCharge {
        bool status;
    };

    struct Time {
        struct std::tm time;
    };


    using Data = std::variant<
            Booted,
            DeviceInfo,
            BatteryLevel,
            BatteryCharge,
            Time
            >;

};

class IGuiBackend {
public:

    virtual bool receiveGuiEvent(AppType::B2GEvent::Data &data) = 0;

    virtual bool sendEventToBackend(const AppType::G2BEvent::Data &data) = 0;

    virtual ~IGuiBackend() = default;

};

} // namespace AppTypes

#endif /* __APP_TYPES_HPP */
