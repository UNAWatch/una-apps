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

#include <stdint.h>
#include <stdbool.h>

#include "touchgfx/Color.hpp"
#include <texts/TextKeysAndLanguages.hpp>
// #include "gui/model/AppTypes.hpp"

#define GUI_CONFIG_MS_2_TICKS(ms) ((ms)/(1000 / Gui::Config::kFrameRate))
namespace Gui
{

namespace Config
{

constexpr uint32_t kFrameRate = 10;
constexpr int32_t kMenuAnimationSteps = 4;
constexpr uint32_t kScreenTimeoutSteps = GUI_CONFIG_MS_2_TICKS(30000);  // 30s
constexpr uint32_t kShutdownScreenTimeout = GUI_CONFIG_MS_2_TICKS(500); // 0.5s;
constexpr uint32_t kLowBattScreenTimeout = GUI_CONFIG_MS_2_TICKS(10*1000);  // 10s;

namespace Button
{
constexpr uint8_t L1 = '1';
constexpr uint8_t L2 = '2';
constexpr uint8_t R1 = '3';
constexpr uint8_t R2 = '4';
constexpr uint8_t L1R2 = 'z';
} // Button

namespace Color
{
// TODO: remove from RAM
static const touchgfx::colortype WHITE = 0xC0C0C0;
static const touchgfx::colortype GRAY = 0x808080;
static const touchgfx::colortype GRAY_DARK = 0x404040;
static const touchgfx::colortype BLACK = 0x000000;
static const touchgfx::colortype YELLOW_DARK = 0xC08000;
static const touchgfx::colortype TEAL = 0x008080;
static const touchgfx::colortype TEAL_DARK = 0x004040;
static const touchgfx::colortype GREEN = 0x00C000;
static const touchgfx::colortype DARK_GREEN = 0x004000;
static const touchgfx::colortype BROUN = 0xC08040;
static const touchgfx::colortype RED = 0xC00000;
static const touchgfx::colortype DARK_RED = 0x400000;
static const touchgfx::colortype BLUE = 0x0000C0;
static const touchgfx::colortype DARK_BLUE = 0x000040;
static const touchgfx::colortype CYAN = 0x40C0C0;
static const touchgfx::colortype CYAN_LIGHT = 0x80C0C0;

} // namespace Color

namespace Battery
{
// Battery Thresholds 0 - Lo - Hi - 100
constexpr uint8_t kThresholdHi = 85;
constexpr uint8_t kThresholdLo = 25;

constexpr uint8_t kLowBatteryThreshold = 5;
} // Battery


namespace DateTime
{
static constexpr touchgfx::TypedTextId DayShort[7] = {
        T_TEXT_SUN, T_TEXT_MON, T_TEXT_TUE, T_TEXT_WED, 
        T_TEXT_THU, T_TEXT_FRI, T_TEXT_SAT };

static constexpr touchgfx::TypedTextId MonthShort[12] = {
        T_TEXT_JAN, T_TEXT_FEB, 
        T_TEXT_MAR, T_TEXT_APR, T_TEXT_MAY, 
        T_TEXT_JUN, T_TEXT_JUL, T_TEXT_AUG, 
        T_TEXT_SEP, T_TEXT_OCT, T_TEXT_NOV, 
        T_TEXT_DEC, };

} // DateTime


} // namespace Config

} // namespace Gui

#endif /* __GUI_CONFIG_HPP */
