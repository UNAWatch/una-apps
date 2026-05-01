#include "Service.hpp"

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_INFO
#include "SDK/UnaLogger/Logger.h"

#include "SDK/Kernel/KernelProviderService.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRate.hpp"
#include "SDK/Messages/CommandMessages.hpp"
#include "SDK/Messages/SensorLayerMessages.hpp"
#include "SDK/Messages/MessageGuard.hpp"

#include "IconHR.h"

Service::Service(SDK::Kernel &kernel)
    : mKernel(kernel)
    , mGlanceUI()
    , mGlanceTitle()
    , mGlanceValue()
    , mIcon()
    , mSensorHR(SDK::Sensor::Type::HEART_RATE)
    , mHrValue(0)
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
    if (!mSensorHR.isConnected()) {
        LOG_DEBUG("Connect to sensors...\n");
        mSensorHR.connect();
    }
}

void Service::disconnect()
{
    if (mSensorHR.isConnected()) {
        LOG_DEBUG("Disconnect from sensors...\n");
        mSensorHR.disconnect();
    }
}

void Service::handleSensorsData(uint16_t handle, SDK::Sensor::DataBatch& data)
{
    if (mSensorHR.matchesDriver(handle)) {
        SDK::SensorDataParser::HeartRate p(data[0]);
        float newValue = p.getBpm();
        LOG_DEBUG("hr = %.2f\n", newValue);
        if (static_cast<uint32_t>(newValue) != static_cast<uint32_t>(mHrValue)) {
            mHrValue = newValue;
            glanceUpdate();
        }
    }
}

void Service::onGlanceTick()
{
    //LOG_DEBUG("Glance tick\n");

    if (mGlanceUI.isInvalid()) {
        if (auto upd = SDK::make_msg<SDK::Message::RequestGlanceUpdate>(mKernel)) {
            upd->name           = APP_NAME;
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
    if (auto gc = SDK::make_msg<SDK::Message::RequestGlanceConfig>(mKernel)) {
        if (gc.send(100) && gc.ok()) {
            if (gc->maxControls >= 3) {
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
    if (mHrValue > 1.0 && !mIsValid) {
        mIsValid = true;

        mGlanceValue.pos({ kValidValueX, kValidValueY },
                         { kValidValueW, kValidValueH })
            .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_30)
            .setText("")
            .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);

    } else if (mHrValue <= 1.0 && mIsValid) {
        mIsValid = false;

        mGlanceValue.pos({ kCalculatingValueX, kCalculatingValueY },
                         { kCalculatingValueW, kCalculatingValueH })
            .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_18)
            .setText(skTextCalculating)
            .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_LEFT);
    }

    if (mIsValid) {
        mGlanceValue.print("%.0f", mHrValue);
    }
}

void Service::createGuiControls()
{
    mGlanceTitle = mGlanceUI.createText();
    mGlanceTitle.pos({ kTitleX, kTitleY }, { kTitleW, kTitleH })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_20)
        .color(GlanceColor_t::GLANCE_COLOR_TEAL)
        .setText("Live HR")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);

    mIcon = mGlanceUI.createImage();
    mIcon.init({ kCalculatingIconX, kCalculatingIconY },
               { ICON_HR_WIDTH, ICON_HR_HEIGHT },
               ICON_HR_ABGR2222);

    mGlanceValue = mGlanceUI.createText();
    mGlanceValue.pos({ kCalculatingValueX, kCalculatingValueY }, { kCalculatingValueW, kCalculatingValueH })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_18)
        .color(GlanceColor_t::GLANCE_COLOR_WHITE)
        .setText(skTextCalculating)
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_LEFT);
}
