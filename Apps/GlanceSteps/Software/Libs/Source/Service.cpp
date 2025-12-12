#include "Service.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include "SDK/Messages/CommandMessages.hpp"
#include "SDK/Messages/SensorLayerMessages.hpp"
#include "SDK/SensorLayer/SensorDataBatch.hpp"

#include "SDK/SensorLayer/DataParsers/SensorDataParserStepCounter.hpp"

#include "icon_60x60.h"
#include "icon_30x30.h"

Service::Service(SDK::Kernel &kernel)
        : mKernel(kernel)
        , mName("Steps")
        , mMaxControls(0)
        , mGlanceUI()
        , mGlanceTitle()
        , mGlanceValue()
        , mSensorPedo(SDK::Sensor::Type::STEP_COUNTER)
{
    LOG_DEBUG("Service\n");
}

Service::~Service()
{
    LOG_DEBUG("~Service\n");

    disconnect();
}

void Service::run()
{
    LOG_DEBUG("Ready\n");

    while (true) {
        SDK::MessageBase *msg;

        // Wait for command (blocks until available)
        if(!mKernel.comm.getMessage(msg)) {
            continue;
        }

        switch (msg->getType()) {

            case SDK::MessageType::EVENT_GLANCE_START:
                LOG_DEBUG("Starting...\n");
                if (configGui()) {
                    createGuiControls();
                    connect();
                }
                break;

            case SDK::MessageType::COMMAND_APP_STOP:
            case SDK::MessageType::EVENT_GLANCE_STOP:
                LOG_DEBUG("Stopping...\n");
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
    LOG_DEBUG("tick\n");
    mSensorPedo.connect();
}

void Service::disconnect()
{
    LOG_DEBUG("tick\n");
    mSensorPedo.disconnect();
}

void Service::onSdlNewData(uint16_t                 handle,
                           const SDK::Sensor::Data* data,
                           uint16_t                 count,
                           uint16_t                 stride)
{
    SDK::Sensor::DataBatch batch(data, count, stride);

    if (mSensorPedo.matchesDriver(handle)) {
        SDK::SensorDataParser::StepCounter p(batch[0]);
        LOG_DEBUG("steps = %d\n", p.getStepCount());
        mGlanceValue.print("%u", p.getStepCount());
    }
}

void Service::onGlanceTick()
{
    LOG_DEBUG("Glance tick\n");

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

bool Service::configGui()
{
    // Get Glance configuration
    auto* gc = mKernel.comm.allocateMessage<SDK::Message::RequestGlanceConfig>();
    if (!gc) {
        mKernel.sys.exit(0); // no return
    }
    mKernel.comm.sendMessage(gc);

    if (gc->getResult() != SDK::MessageResult::SUCCESS) {
        LOG_ERROR("Command execution status: %s\n", gc->getResultStr());
        mKernel.comm.releaseMessage(gc);
        return false;
    }

    mGlanceUI.setWidth(gc->width);
    mGlanceUI.setHeight(gc->height);
    mMaxControls = gc->maxControls;
    mKernel.comm.releaseMessage(gc);

    return true;
}

void Service::createGuiControls()
{
    mGlanceUI.createImage().init({20, 0}, {60, 60}, ICON_60X60_ABGR2222);

    mGlanceTitle = mGlanceUI.createText();
    mGlanceTitle.pos({ 70, 0 }, { 100, 25 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_20)
        .color(GlanceColor_t::GLANCE_COLOR_TEAL)
        .setText("Steps")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);

    mGlanceValue = mGlanceUI.createText();
    mGlanceValue.pos({ 80, 28 }, { 80, 34 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_30)
        .color(GlanceColor_t::GLANCE_COLOR_WHITE)
        .setText("")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);
}
