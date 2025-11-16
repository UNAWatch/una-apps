#include "Service.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include "icon_60x60.h"
#include "icon_30x30.h"
#include "SDK/SensorLayer/DataParsers/SensorDataParserFloorCounter.hpp"

Service::Service()
        : mKernel(SDK::KernelProviderService::GetInstance().getKernel())
        , mTerminate(false)
        , mGlanceUI()
        , mGlanceTitle()
        , mGlanceValue()
        , mFloorsSensor(SDK::Sensor::Type::FLOOR_COUNTER, this, 1000, 1000)
        , mFloorsValue(0)
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
    //mKernel.system.delay(800);
    mFloorsSensor.connect();

    while (!mTerminate) {
        mKernel.system.delay(1000);
    }

    mFloorsSensor.disconnect();
}

SDK::Interface::IGlance::Info Service::glanceGetInfo()
{
    IGlance::Info info{};
    info.altname = "Floors";
    info.count = static_cast<uint8_t>(mGlanceUI.size());
    info.ctrls = mGlanceUI.data();
    return info;
}

void Service::glanceUpdate()
{
    mGlanceValue.print("%u", mFloorsValue);
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

    if (mFloorsSensor.matchesDriver(sensor)) {
        SDK::SensorDataParser::FloorCounter f {sample};
        if (f.isDataValid()) {
            mFloorsValue = f.getFloorsDown() + f.getFloorsUp();
        }
    }

}

void Service::createGlanceGUI()
{
    mGlanceUI.createImage().init({20, 0}, {60, 60}, ICON_60X60_ABGR2222);

    mGlanceTitle = mGlanceUI.createText();
    mGlanceTitle.pos({ 70, 0 }, { 100, 25 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_20)
        .color(GlanceColor_t::GLANCE_COLOR_TEAL)
        .setText("Floors")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);

    mGlanceValue = mGlanceUI.createText();
    mGlanceValue.pos({ 80, 28 }, { 80, 34 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_30)
        .color(GlanceColor_t::GLANCE_COLOR_WHITE)
        .setText("")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);

}
