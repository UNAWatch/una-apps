#include "Service.hpp"
#include "Clock24.h"

#include <stdio.h>
#include <time.h>

#define LOG_MODULE_PRX      "Service::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

Service::Service(const IKernel& kernel)
        : mKernel(kernel)
        , mTerminate(false)
        , mUI(mKernel.app.getGlanceArea().w, mKernel.app.getGlanceArea().h)
        , mTextTime()
        , mTextTemperature()
        , mTemperature(23.5)
        , mUpdateTimer(1000)
{
    mKernel.app.registerApp(this);
    mKernel.app.registerGlance(this);
    mKernel.app.initialized();

    createUI();
}

void Service::run()
{
    while (!mTerminate) {
        mKernel.app.delay(1000);
    }
}

GlanceControl_t* Service::glanceGetControls(uint8_t& count)
{
    count = mUI.size();

    return mUI.data();
}

void Service::glanceUpdate()
{
    if (!mUpdateTimer.check()) {
        return;
    }

    mTemperature += 0.1;
    if (mTemperature > 25) {
        mTemperature = 23.5;
    }

    mTextTemperature.print("%.2f", mTemperature);

    std::time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);

    mTextTime.print("%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void Service::glanceClose()
{

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
    SDK::Glance::ControlRectangle panelClock = mUI.createRect().init({0,   0},
                                                                     {25, 25},
                                                                     GlanceColor_t::GLANCE_COLOR_BLUE,
                                                                     GlanceColor_t::GLANCE_COLOR_BLACK);

    mUI.createRect().init({25,  0},
                          {75, 25},
                          GlanceColor_t::GLANCE_COLOR_BLUE,
                          GlanceColor_t::GLANCE_COLOR_BLACK);

    mUI.createRect().init({0,  25},
                          {50, 25},
                          GlanceColor_t::GLANCE_COLOR_BLUE,
                          GlanceColor_t::GLANCE_COLOR_RED);

    mUI.createRect().init({50, 25},
                          {50, 25},
                          GlanceColor_t::GLANCE_COLOR_BLUE,
                          GlanceColor_t::GLANCE_COLOR_GREEN);

    GlanceSize_t  clockImageSize = {CLOCK24_WIDTH, CLOCK24_HEIGHT};
    GlancePoint_t clockImagePos  = SDK::Glance::Align::placeWithin(panelClock.pos(),
                                                                   panelClock.size(),
                                                                   clockImageSize,
                                                                   GlanceAlignH_t::GLANCE_ALIGN_H_CENTER,
                                                                   GlanceAlignV_t::GLANCE_ALIGN_V_CENTER);

    mUI.createImage().init(clockImagePos, clockImageSize, CLOCK24_ABGR2222);

    mUI.createText().init({25, 75},
                          "Temp, °C:",
                          GlanceFont_t::GLANCE_FONT_POPPINS_REGULAR_18,
                          GlanceColor_t::GLANCE_COLOR_WHITE);

    mTextTime = mUI.createText();
    mTextTime.pos({5, 30})
             .color(GlanceColor_t::GLANCE_COLOR_YELLOW_DARK)
             .setText("00:00:00");

    mTextTemperature = mUI.createText();
    mTextTemperature.pos({60, 30})
                    .color(GlanceColor_t::GLANCE_COLOR_WHITE)
                    .setText("23.5");
}
