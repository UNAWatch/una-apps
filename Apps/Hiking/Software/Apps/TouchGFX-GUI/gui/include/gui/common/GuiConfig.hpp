/**
 ******************************************************************************
 * @file    Config.hpp
 * @date    19-01-2024
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Configuration for GUI.
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef __GUI_CONFIG_HPP
#define __GUI_CONFIG_HPP

#include <cstdint>
#include <cstdbool>
#include <ctime>
#include <cmath>

#include "touchgfx/Color.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include "SDK/Interfaces/IApp.hpp"

#include "AppMenu.hpp"
#include "AppUtils.hpp"

#define GUI_CONFIG_MS_2_TICKS(ms) ((ms)/(1000 / Gui::Config::kFrameRate))
namespace Gui
{

namespace Config
{

constexpr uint32_t kFrameRate = SDK::Interface::IApp::kFrameRate;
constexpr uint32_t kMenuAnimationSteps = 4;
constexpr uint32_t kScreenTimeoutSteps = GUI_CONFIG_MS_2_TICKS(30 * 1000);              // 30s
constexpr uint32_t kConfirmTimeout = GUI_CONFIG_MS_2_TICKS(1 * 1000);                   // 1s
constexpr uint32_t kTrackLapScreenTimeout = GUI_CONFIG_MS_2_TICKS(10 * 1000);           // 10s
constexpr uint32_t kTrackTitleInfoSwitchPeriod = GUI_CONFIG_MS_2_TICKS(3 * 1000);       // 3s
constexpr uint32_t kTrackActionConfirmScreenTimeout = GUI_CONFIG_MS_2_TICKS(2 * 1000);  // 2s

namespace Button
{
constexpr uint8_t L1 = SDK::Interface::IApp::BUTTON_L1;
constexpr uint8_t L2 = SDK::Interface::IApp::BUTTON_L2;
constexpr uint8_t R1 = SDK::Interface::IApp::BUTTON_R1;
constexpr uint8_t R2 = SDK::Interface::IApp::BUTTON_R2;
constexpr uint8_t L1R2 = SDK::Interface::IApp::BUTTON_L1R2;
} // Button

namespace Battery
{
// Battery Thresholds 0 - Lo - Hi - 100
constexpr uint8_t kThresholdHi = 85;
constexpr uint8_t kThresholdLo = 25;
constexpr uint8_t kLowBatteryThreshold = 5;
}; // Battery

namespace Color
{
inline const touchgfx::colortype WHITE = 0xC0C0C0;
inline const touchgfx::colortype GRAY = 0x808080;
inline const touchgfx::colortype GRAY_DARK = 0x404040;
inline const touchgfx::colortype BLACK = 0x000000;
inline const touchgfx::colortype YELLOW_DARK = 0xC08000;
inline const touchgfx::colortype TEAL = 0x008080;
inline const touchgfx::colortype TEAL_DARK = 0x004040;
inline const touchgfx::colortype GREEN = 0x00C000;
inline const touchgfx::colortype DARK_GREEN = 0x004000;
inline const touchgfx::colortype BROUN = 0xC08040;
inline const touchgfx::colortype RED = 0xC00000;
inline const touchgfx::colortype DARK_RED = 0x400000;
inline const touchgfx::colortype BLUE = 0x0000C0;
inline const touchgfx::colortype DARK_BLUE = 0x000040;
inline const touchgfx::colortype CYAN = 0x40C0C0;
inline const touchgfx::colortype CYAN_LIGHT = 0x80C0C0;

inline const touchgfx::colortype MENU_NOT_SELECTED_TIP_ON = YELLOW_DARK;
inline const touchgfx::colortype MENU_NOT_SELECTED_TIP_OFF = TEAL;

} // namespace Color

} // namespace Config

} // namespace Gui

#endif /* __GUI_CONFIG_HPP */
