
#pragma once

#include "SDK/Messages/MessageBase.hpp"
#include "SDK/Messages/MessageTypes.hpp"
#include "SDK/Messages/CommandMessages.hpp"
#include "SDK/Messages/MessageGuard.hpp"
#include "SDK/Kernel/Kernel.hpp"

#include <array>

// Force 4-byte alignment for all message structures
#pragma pack(push, 4)

namespace CustomMessage {

    ///////////////////////////////////////
    //// Application custom commands
    ///////////////////////////////////////

    // Service --> GUI
    constexpr SDK::MessageType::Type HR_VALUES = 0x00000001;

    ///////////////////////////////////////
    //// Application custom structures
    ///////////////////////////////////////

    // Service --> GUI
    struct HRValues : public SDK::MessageBase {
        float heartRate;
        float trustLevel;

        HRValues()
            : SDK::MessageBase(HR_VALUES)
            , heartRate()
            , trustLevel()
        {}
    };


    ///////////////////////////////////////
    //// Wrappers
    ///////////////////////////////////////

    class ServiceSender {
    public:
        ServiceSender(SDK::Kernel& kernel) : mKernel(kernel)
        {}

        virtual ~ServiceSender() = default;

        // Service --> GUI
        bool publishHeartRate(float value, float trustLevel)
        {
            if (auto req = SDK::make_msg<CustomMessage::HRValues>(mKernel)) {
                req->heartRate  = value;
                req->trustLevel = trustLevel;

                return req.send();
            }

            return false;
        }

    private:
        SDK::Kernel& mKernel;
    };

} // namespace CustomMessage

#pragma pack(pop)
