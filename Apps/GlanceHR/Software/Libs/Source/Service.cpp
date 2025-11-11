#include "Service.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include "icon_60x60.h"
#include "icon_30x30.h"
#include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRate.hpp"

Service::Service()
        : mKernel(SDK::KernelProviderService::GetInstance().getKernel())
        , mTerminate(false)
        , mGlanceUI()
        , mGlanceTitle()
        , mGlanceValue()
        , mHrSensor(SDK::Sensor::Type::HEART_RATE, this, 1000, 1000)
        , mHrValue(0)
        , mIsValid(false)
{
    int16_t w;
    int16_t h;
    mKernel.app.getGlanceArea(w, h);
    mGlanceUI.setWidth(w);
    mGlanceUI.setHeight(h);

    mKernel.app.registerGlance(this);

    createGlanceGUI();
}

void Service::run()
{
    mHrSensor.connect();

    while (!mTerminate) {
        mKernel.system.delay(1000);
    }

    mHrSensor.disconnect();
}

SDK::Interface::IGlance::Info Service::glanceGetInfo()
{
    IGlance::Info info{};
    info.altname = "Live HR";
    info.count = static_cast<uint8_t>(mGlanceUI.size());
    info.ctrls = mGlanceUI.data();
    return info;
}

void Service::glanceUpdate()
{
    if (mHrValue > 1.0 && !mIsValid) {
        mIsValid = true;
        mGlanceValue.pos({ 80, 28 }, { 160, 34 })
                .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_30)
                .setText("")
                .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);
    } else if (mHrValue <= 1.0 && mIsValid) {
        mIsValid = false;
        mGlanceValue.pos({ 81, 34 }, { 130, 23 })
                .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_18)
                .setText(skTextCalculating)
                .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_LEFT);
    }

    if (mIsValid) {
        mGlanceValue.print("%.0f          ", mHrValue);
    }
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

void Service::onSdlNewData(const SDK::Interface::ISensorDriver*              sensor,
                           const std::vector< SDK::Interface::ISensorData*>& data,
                           bool                                              first)
{
    // We are only interested in the last sample
    const size_t sampleNum = data.size();
    if (sampleNum == 0) {
        return;
    }

    const SDK::Interface::ISensorData& sample = *data[sampleNum - 1];

    if (mHrSensor.matchesDriver(sensor)) {
        SDK::SensorDataParser::HeartRate hr {sample};
        if (hr.isDataValid()) {
            mHrValue = hr.getBpm();
        }
    }

}

void Service::createGlanceGUI()
{
    mGlanceUI.createImage().init({35, 30}, {30, 30}, ICON_30X30_ABGR2222);

    mGlanceTitle = mGlanceUI.createText();
    mGlanceTitle.pos({ 81, 0 }, { 125, 25 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_20)
        .color(GlanceColor_t::GLANCE_COLOR_TEAL)
        .setText("Live HR")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_LEFT);

    mGlanceValue = mGlanceUI.createText();
    mGlanceValue.pos({ 81, 34 }, { 172, 23 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_18)
        .color(GlanceColor_t::GLANCE_COLOR_WHITE)
        .setText(skTextCalculating)
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_LEFT);

}
