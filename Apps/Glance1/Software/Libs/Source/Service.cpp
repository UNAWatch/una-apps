#include "Service.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"
#include "Clock24.h"

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

Service::Service()
        : mKernel(SDK::KernelProviderService::GetInstance().getKernel())
        , mTerminate(false)
        , mUI()
        , mTextTime()
        , mTextTemperature()
        , mTemperature(23.5)
        , mTime()
{
    int16_t w;
    int16_t h;
    mKernel.app.getGlanceArea(w, h);
    mUI.setWidth(w);
    mUI.setHeight(h);

    mKernel.app.registerGlance(this);

    std::time_t t = time(NULL);
    localtime_r(&t, &mTime);

    createUI();
}

void Service::run()
{
    while (!mTerminate) {

        mTemperature += 0.1;
        if (mTemperature > 25) {
            mTemperature = 23.5;
        }

        std::time_t t = time(NULL);
        localtime_r(&t, &mTime);

        mKernel.system.delay(1000);
    }
}

SDK::Interface::IGlance::Info Service::glanceGetInfo()
{
    IGlance::Info info{};
    info.altname = "glance1";
    info.count = static_cast<uint8_t>(mUI.size());
    info.ctrls = mUI.data();
    return info;
}

void Service::glanceUpdate()
{
    mTextTemperature.print("%.1f", mTemperature);
    mTextTime.print("%02d:%02d:%02d", mTime.tm_hour, mTime.tm_min, mTime.tm_sec);
}

void Service::glanceClose()
{
    LOG_INFO("called\n");
    mTerminate = true;
}

void Service::onCreate()
{
    LOG_INFO("called\n");
}

void Service::onStart()
{
    LOG_INFO("called\n");
}

void Service::onResume()
{
    LOG_INFO("called\n");
}

void Service::onStop()
{
    LOG_INFO("called\n");
    mTerminate = true;
}

void Service::onPause()
{
    LOG_INFO("called\n");
}

void Service::onDestroy()
{
    LOG_INFO("called\n");
}

void Service::createUI()
{
    // first row
    mUI.createRect().init( {50, 0}, {140, 30},
            GlanceColor_t::GLANCE_COLOR_BLUE,
            GlanceColor_t::GLANCE_COLOR_BLACK);

    SDK::Glance::ControlRectangle panelClock = mUI.createRect().init(
            {50, 0}, {30, 30},
            GlanceColor_t::GLANCE_COLOR_BLUE,
            GlanceColor_t::GLANCE_COLOR_BLACK);

    GlanceSize_t clockImageSize = {CLOCK24_WIDTH, CLOCK24_HEIGHT};
    GlancePoint_t clockImagePos = SDK::Glance::Align::placeWithin(
            panelClock.pos(),
            panelClock.size(),
            clockImageSize,
            GlanceAlignH_t::GLANCE_ALIGN_H_CENTER,
            GlanceAlignV_t::GLANCE_ALIGN_V_CENTER);

    mUI.createImage().init(clockImagePos, clockImageSize, CLOCK24_ABGR2222);

    mTextTime = mUI.createText();
    mTextTime.pos( {80, 2}, {110, 26})
            .color(GlanceColor_t::GLANCE_COLOR_YELLOW_DARK)
            .setText("00:00:00")
            .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);

    // second row
    mUI.createRect().init( {50, 30}, {95, 30},
            GlanceColor_t::GLANCE_COLOR_BLUE, GlanceColor_t::GLANCE_COLOR_RED);

    mUI.createText().init( {50, 32}, {90, 26},
            "Temp, C:",
            GlanceFont_t::GLANCE_FONT_POPPINS_REGULAR_18,
            GlanceColor_t::GLANCE_COLOR_WHITE,
            GlanceAlignH_t::GLANCE_ALIGN_H_RIGHT);

    mUI.createRect().init( {145, 30}, {45, 30},
            GlanceColor_t::GLANCE_COLOR_BLUE,
            GlanceColor_t::GLANCE_COLOR_GREEN);

    mTextTemperature = mUI.createText();
    mTextTemperature.pos( {150, 32}, {40, 26})
            .color(GlanceColor_t::GLANCE_COLOR_WHITE)
            .setText("23.5")
            .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);
}
