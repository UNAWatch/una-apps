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
        , mName("AVG / R HR")
        , mMaxControls(0)
        , mGlanceUI()
        , mGlanceTitle()
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
    // |-------------|
    // | AHR / R HR  |
    // |------|------|
    // |  v   |  v   |
    // |-------------|


    mGlanceTitle = mGlanceUI.createText();

    mGlanceTitle
        .pos({ 20, 0 }, { 200, 25 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_20)
        .color(GlanceColor_t::GLANCE_COLOR_TEAL)
        .setText("AVG / R HR")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);



    mGlanceValueAHR = mGlanceUI.createText();

    mGlanceValueAHR
        .pos({ 44, 28 }, { 80, 34 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_30)
        .color(GlanceColor_t::GLANCE_COLOR_WHITE)
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);



    mGlanceValueRHR = mGlanceUI.createText();

    mGlanceValueRHR
        .pos({ 116, 28 }, { 80, 34 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_30)
        .color(GlanceColor_t::GLANCE_COLOR_WHITE)
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);
}
