#include "Service.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_INFO
#include "SDK/UnaLogger/Logger.h"

#include "SDK/Messages/CommandMessages.hpp"
#include "SDK/Messages/SensorLayerMessages.hpp"
#include "SDK/SensorLayer/SensorDataBatch.hpp"

#include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRateMetrics.hpp"

#include "icon_60x60.h"
#include "icon_30x30.h"

Service::Service(SDK::Kernel &kernel)
        : mKernel(kernel)
        , mName("HeartRate")
        , mMaxControls(0)
        , mGlanceUI()
        , mGlanceTitleAHR()
        , mGlanceTitleRHR()
        , mGlanceValueAHR()
        , mGlanceValueRHR()
        , mSensorHRMetrics(SDK::Sensor::Type::HEART_RATE_METRICS)
        , mAHRValue(0)
        , mRHRValue(0)
        , mIsValid(false)
{
}

Service::~Service()
{
    disconnect();
}

void Service::run()
{
    LOG_INFO("Started\n");

    while (true) {
        SDK::MessageBase *msg;

        // Wait for command (blocks until available)
        if(!mKernel.comm.getMessage(msg)) {
            continue;
        }

        switch (msg->getType()) {

            case SDK::MessageType::EVENT_GLANCE_START:
                LOG_INFO("GLANCE is now running\n");
                if (configGui()) {
                    createGuiControls();
                    connect();
                } else {
                    mKernel.comm.releaseMessage(msg);
                    mKernel.sys.exit(0); // no return
                }
                break;

            case SDK::MessageType::COMMAND_APP_STOP:
                LOG_INFO("Force exit from the application\n");
            case SDK::MessageType::EVENT_GLANCE_STOP:
                LOG_INFO("GLANCE has stopped\n");
                disconnect();
                // We must release message because this is the last event.
                mKernel.comm.releaseMessage(msg);
                return;
                break;

            case SDK::MessageType::EVENT_GLANCE_TICK:
                onGlanceTick();
                break;

            case SDK::MessageType::EVENT_SENSOR_LAYER_DATA: {
                auto event = static_cast<SDK::Message::Sensor::EventData*>(msg);
                this->onSdlNewData(event->handle,
                                   event->data,
                                   event->count,
                                   event->stride);
                } break;

            default:
                break;
        }

        // Release message after processing
        mKernel.comm.releaseMessage(msg);
    }
}

void Service::connect()
{
    if (!mSensorHRMetrics.isConnected()) {
        LOG_DEBUG("Connect to sensors...\n");
        mSensorHRMetrics.connect();
    }
}

void Service::disconnect()
{
    if (mSensorHRMetrics.isConnected()) {
        LOG_DEBUG("Disconnect from sensors...\n");
        mSensorHRMetrics.disconnect();
    }
}

void Service::onSdlNewData(uint16_t                 handle,
                           const SDK::Sensor::Data* data,
                           uint16_t                 count,
                           uint16_t                 stride)
{
    SDK::Sensor::DataBatch batch(data, count, stride);

    if (mSensorHRMetrics.matchesDriver(handle)) {
        SDK::SensorDataParser::HeartRateMetrics p(batch[0]);
        mAHRValue = p.getAhr();
        mRHRValue = p.getRhr();
        glanceUpdate();
    }
}

void Service::onGlanceTick()
{
    //LOG_DEBUG("Glance tick\n");

    if (mGlanceUI.isInvalid()) {
        auto* upd = mKernel.comm.allocateMessage<SDK::Message::RequestGlanceUpdate>();
        if (upd) {
            upd->name = mName;
            upd->controls = mGlanceUI.data();
            upd->controlsNumber = static_cast<uint32_t>(mGlanceUI.size());

            mKernel.comm.sendMessage(upd, 100);
            mKernel.comm.releaseMessage(upd);
        }

        mGlanceUI.setValid();
   }
}
//"240X60"
bool Service::configGui()
{
    bool status = false;
    // Get Glance configuration
    auto* gc = mKernel.comm.allocateMessage<SDK::Message::RequestGlanceConfig>();
    if (gc) {
        if (mKernel.comm.sendMessage(gc, 100) && gc->getResult() == SDK::MessageResult::SUCCESS) {
            if (gc->maxControls >= 3) {
                mMaxControls = gc->maxControls;
                mGlanceUI.setWidth(gc->width);
                mGlanceUI.setHeight(gc->height);
                status = true;
            }
        }
        mKernel.comm.releaseMessage(gc);
    }

    return status;
}

void Service::glanceUpdate()
{
    mGlanceValueAHR.print("%.0f", mAHRValue);
    mGlanceValueRHR.print("%.0f", mRHRValue);
}

void Service::createGuiControls()
{
    // |-----------|
    // | AHR | RHR |
    // |-----|-----|
    // |  v  |  v  |
    // |-----------|

    //////////////////////////////////////////////////////////////
    //// Sizes
    //////////////////////////////////////////////////////////////

    GlancePoint_t p11 = {
            .x = 0,
            .y = 0
    };

    GlancePoint_t p12 = {
            .x = static_cast<uint16_t>(mGlanceUI.getWidth() / 2),
            .y = 0
    };

    GlancePoint_t p21 = {
            .x = 0,
            .y = static_cast<uint16_t>(mGlanceUI.getHeight() / 2),
    };

    GlancePoint_t p22 = {
            .x = static_cast<uint16_t>(mGlanceUI.getWidth() / 2),
            .y = static_cast<uint16_t>(mGlanceUI.getHeight() / 2),
    };

    GlanceSize_t areaSize = {
            .w = static_cast<uint16_t>(mGlanceUI.getWidth() / 2),
            .h = static_cast<uint16_t>(mGlanceUI.getHeight() / 2),
    };

    //////////////////////////////////////////////////////////////
    //// Title AHR
    //////////////////////////////////////////////////////////////

    mGlanceTitleAHR = mGlanceUI.createText();

    GlancePoint_t posGlanceTitleAHR = SDK::Glance::Align::placeWithin(p11,
                                                                      areaSize,
                                                                      areaSize,
                                                                      GlanceAlignH_t::GLANCE_ALIGN_H_CENTER,
                                                                      GlanceAlignV_t::GLANCE_ALIGN_V_CENTER);

    mGlanceTitleAHR
        .pos(posGlanceTitleAHR, areaSize)
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_20)
        .color(GlanceColor_t::GLANCE_COLOR_TEAL)
        .setText("AHR")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);

    //////////////////////////////////////////////////////////////
    //// Title RHR
    //////////////////////////////////////////////////////////////

    mGlanceTitleRHR = mGlanceUI.createText();

    GlancePoint_t posGlanceTitleRHR = SDK::Glance::Align::placeWithin(p12,
                                                                      areaSize,
                                                                      areaSize,
                                                                      GlanceAlignH_t::GLANCE_ALIGN_H_CENTER,
                                                                      GlanceAlignV_t::GLANCE_ALIGN_V_CENTER);

    mGlanceTitleRHR
        .pos(posGlanceTitleRHR, areaSize)
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_20)
        .color(GlanceColor_t::GLANCE_COLOR_TEAL)
        .setText("RHR")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);

    //////////////////////////////////////////////////////////////
    //// Value AHR
    //////////////////////////////////////////////////////////////

    mGlanceValueAHR = mGlanceUI.createText();

    GlancePoint_t posGlanceValueAHR = SDK::Glance::Align::placeWithin(p21,
                                                                      areaSize,
                                                                      areaSize,
                                                                      GlanceAlignH_t::GLANCE_ALIGN_H_CENTER,
                                                                      GlanceAlignV_t::GLANCE_ALIGN_V_CENTER);

    mGlanceValueAHR
        .pos(posGlanceValueAHR, areaSize)
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_20)
        .color(GlanceColor_t::GLANCE_COLOR_TEAL)
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);

    //////////////////////////////////////////////////////////////
    //// Value RHR
    //////////////////////////////////////////////////////////////

    mGlanceValueRHR = mGlanceUI.createText();

    GlancePoint_t posGlanceValueRHR = SDK::Glance::Align::placeWithin(p22,
                                                                      areaSize,
                                                                      areaSize,
                                                                      GlanceAlignH_t::GLANCE_ALIGN_H_CENTER,
                                                                      GlanceAlignV_t::GLANCE_ALIGN_V_CENTER);

    mGlanceValueRHR
        .pos(posGlanceValueRHR, areaSize)
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_20)
        .color(GlanceColor_t::GLANCE_COLOR_TEAL)
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);
}
