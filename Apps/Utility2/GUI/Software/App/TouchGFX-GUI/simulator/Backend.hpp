
#include <queue>
#include <chrono>
#include <ctime>

#include "touchgfx/UIEventListener.hpp"

#include "gui/model/AppTypes.hpp"
#include "gui/common/GuiLogger.hpp"


#ifndef BACKEND_HPP
#define BACKEND_HPP


class Backend : virtual public AppType::IGuiBackend, public touchgfx::UIEventListener {

public:

    static Backend &GetInstance()
    {
        static Backend sInstance;
        return sInstance;
    }

    // Tick
    virtual void handleTickEvent() override
    {
        updTime();
       

        while (!mGuiRxQueue.empty()) {
            auto data = mGuiRxQueue.front();
            mGuiRxQueue.pop();

            std::visit([this](auto &&value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::monostate>) {
                    break;
                } else  if constexpr (std::is_same_v<T, AppType::G2BEvent::Shutdown>) {
                    LOG_DEBUG("Shutdown\n");
                } else  if constexpr (std::is_same_v < T, AppType::G2BEvent::VibroShort>) {
                    LOG_DEBUG("VibroShort\n");
                } else  if constexpr (std::is_same_v < T, AppType::G2BEvent::VibroLong>) {
                    LOG_DEBUG("VibroLong\n");
                } else  if constexpr (std::is_same_v < T, AppType::G2BEvent::BuzzShort>) {
                    LOG_DEBUG("BuzzShort\n");
                } else  if constexpr (std::is_same_v < T, AppType::G2BEvent::BuzzLong>) {
                    LOG_DEBUG("BuzzLong\n");
                } else  if constexpr (std::is_same_v < T, AppType::G2BEvent::Backlight>) {
                    LOG_DEBUG("Backlight %u\n", static_cast<AppType::G2BEvent::Backlight>(value).status);
                }
            }, data);
        }

    }

    // Keys
    virtual void handleKeyEvent(uint8_t c) override
    {

        LOG_DEBUG("%c\n", c);

        // Battery level
        if ('+' == c) {
            if (mBatteryLevel < 100) { 
                mBatteryLevel++;
                mGuiTxQueue.emplace(AppType::B2GEvent::BatteryLevel {static_cast<float>(mBatteryLevel)});
            }
        }
        if ('-' == c) {
            if (mBatteryLevel > 0) {
                mBatteryLevel--;
                mGuiTxQueue.emplace(AppType::B2GEvent::BatteryLevel {static_cast<float>(mBatteryLevel)});
            }
        }

        // Charging
        if ('C' == c) {
            mCharging = !mCharging;
            mGuiTxQueue.emplace(AppType::B2GEvent::BatteryCharge {mCharging});
        }
        

    }





    virtual bool receiveGuiEvent(AppType::B2GEvent::Data &data) override
    {
        if (!mGuiTxQueue.empty()) {
            data = mGuiTxQueue.front();
            mGuiTxQueue.pop();
            return true;
        }
        return false;
    }

    virtual bool sendEventToBackend(const AppType::G2BEvent::Data &data) override
    {
        mGuiRxQueue.emplace(data);
        return true;
    }

private:
    Backend() { 
        // Initial setup
        startTimeS = timeNowUTC();
        updTime();

        // Device info
        mDeviceInfo.name = "Demo";
        mDeviceInfo.manufacturer = "UNA";
        mDeviceInfo.id = "654321";
        mDeviceInfo.serialNumber = "ABCDEFABCDEFABCDEFABCDEF";
        mDeviceInfo.swVersion = "0.0.0";

        mGuiTxQueue.emplace(AppType::B2GEvent::DeviceInfo {mDeviceInfo});

        // Battery
        mGuiTxQueue.emplace(AppType::B2GEvent::BatteryLevel {static_cast<float>(mBatteryLevel)});



        // Final
        mGuiTxQueue.emplace(AppType::B2GEvent::Booted {});
    }
    
    virtual ~Backend() = default;

    std::queue<AppType::G2BEvent::Data> mGuiRxQueue;
    std::queue<AppType::B2GEvent::Data> mGuiTxQueue;

    // Time
    time_t startTimeS = 0;
    std::tm mTime;

    time_t timeNowUTC() { 
        return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }

    std::tm getSystemTime()
    {
        auto currentTime = std::chrono::system_clock::now();

        auto time = std::chrono::system_clock::to_time_t(currentTime);
        std::tm localTime;
        localtime_s(&localTime, &time);
        return localTime;
    }

    void updTime() { 
        std::tm timeNow = getSystemTime();
        if (timeNow.tm_hour != mTime.tm_hour || timeNow.tm_min != mTime.tm_min || timeNow.tm_sec != mTime.tm_sec) {
            mTime = timeNow;
            mGuiTxQueue.emplace(AppType::B2GEvent::Time {mTime});
        }
    }

    // Device info
    AppType::DeviceInfo mDeviceInfo {};

    // Battery
    uint8_t mBatteryLevel = 73;
    bool mCharging = false;


};

#endif // BACKEND_HPP
