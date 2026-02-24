#include "Service.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_INFO
#include "SDK/UnaLogger/Logger.h"

#include "SDK/Messages/CommandMessages.hpp"
#include "SDK/Messages/SensorLayerMessages.hpp"
#include "SDK/SensorLayer/SensorDataBatch.hpp"
#include "SDK/Messages/MessageGuard.hpp"

#include "SDK/SensorLayer/DataParsers/SensorDataParserBatteryLevel.hpp"

#include "icon_60x60.h"

Service::Service(SDK::Kernel &kernel)
    : mKernel(kernel)
    , mName("Battery")
    , mMaxControls(0)
    , mGlanceUI()
    , mGlanceTitle()
    , mGlanceValue()
    , mSensorSOC(SDK::Sensor::Type::BATTERY_LEVEL)
    , mSOCValue(0)
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
                SDK::Sensor::DataBatch batch(event->data, event->count, event->stride);
                handleSensorsData(event->handle, batch);
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
    if (!mSensorSOC.isConnected()) {
        LOG_DEBUG("Connect to sensors...\n");
        mSensorSOC.connect();
    }
}

void Service::disconnect()
{
    if (mSensorSOC.isConnected()) {
        LOG_DEBUG("Disconnect from sensors...\n");
        mSensorSOC.disconnect();
    }
}

void Service::handleSensorsData(uint16_t handle, SDK::Sensor::DataBatch& data)
{
    if (mSensorSOC.matchesDriver(handle)) {
        SDK::SensorDataParser::BatteryLevel p(data[0]);
        if (p.isDataValid()) {
            float value = p.getCharge();
            if (value > 100.0) {
                value = 100.0;
            }

            LOG_DEBUG("level = %.2f\n", value);

            mSOCValue = value;
            glanceUpdate();
        }
    }
}

void Service::onGlanceTick()
{
    //LOG_DEBUG("Glance tick\n");

    if (mGlanceUI.isInvalid()) {
        auto upd = SDK::make_msg<SDK::Message::RequestGlanceUpdate>(mKernel);
        if (upd) {
            upd->name           = mName;
            upd->controls       = mGlanceUI.data();
            upd->controlsNumber = static_cast<uint32_t>(mGlanceUI.size());
            upd.send(100);
        }

        mGlanceUI.setValid();
   }
}

bool Service::configGui()
{
    bool status = false;
    // Get Glance configuration
    auto gc = SDK::make_msg<SDK::Message::RequestGlanceConfig>(mKernel);
    if (gc) {
        if (gc.send(100) && gc.ok()) {
            if (gc->maxControls >= 3) {
                mMaxControls = gc->maxControls;
                mGlanceUI.setWidth(gc->width);
                mGlanceUI.setHeight(gc->height);
                status = true;
            }
        }
    }

    return status;
}

void Service::glanceUpdate()
{
    mGlanceValue.print("%.0f%%", mSOCValue);
}

void Service::createGuiControls()
{
    mGlanceUI.createImage().init({20, 0}, {60, 60}, ICON_60X60_ABGR2222);

    mGlanceTitle = mGlanceUI.createText();
    mGlanceTitle.pos({ 70, 0 }, { 100, 24 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_20)
        .color(GlanceColor_t::GLANCE_COLOR_TEAL)
        .setText("Battery")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);

    mGlanceValue = mGlanceUI.createText();
    mGlanceValue.pos({ 72, 25 }, { 100, 49 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_30)
        .color(GlanceColor_t::GLANCE_COLOR_WHITE)
        .setText("")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);
}
