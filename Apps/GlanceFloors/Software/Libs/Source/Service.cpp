#include "Service.hpp"

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include "SDK/Messages/CommandMessages.hpp"

#include "icon_60x60.h"
#include "icon_30x30.h"
//#include "SDK/SensorLayer/DataParsers/SensorDataParserFloorCounter.hpp"

Service::Service(SDK::Kernel &kernel)
        : mKernel(kernel)
        , mName("Floors")
        , mMaxControls(0)
        , mGlanceUI()
        , mGlanceTitle()
        , mGlanceValue()
        //, mFloorsSensor(SDK::Sensor::Type::FLOOR_COUNTER, this, 1000, 1000)
        , mFloorsValue(0)
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
        mKernel.sys.exit(0); // no return
    }

    mGlanceUI.setWidth(gc->width);
    mGlanceUI.setHeight(gc->height);
    mMaxControls = gc->maxControls;
    mKernel.comm.releaseMessage(gc);

    createGlanceGUI();
}

void Service::run()
{
    LOG_DEBUG("Ready\n");

    //mFloorsSensor.connect();

    while (true) {
        SDK::MessageBase *msg;

        // Wait for command (blocks until available)
        if(!mKernel.comm.getMessage(msg)) {
            continue;
        }

        switch (msg->getType()) {

            case SDK::MessageType::COMMAND_APP_STOP:
                onStop(msg);
                break;

            case SDK::MessageType::EVENT_GLANCE_START:
                onGlanceStart(msg);
                break;

            case SDK::MessageType::EVENT_GLANCE_STOP:
                onGlanceStop(msg);
                break;

            case SDK::MessageType::EVENT_GLANCE_TICK:
                onGlanceTick(msg);
                break;
        }

        // Release message after processing
        mKernel.comm.releaseMessage(msg);
    }

    //mFloorsSensor.disconnect();
}

void Service::onStop(SDK::MessageBase *msg)
{
    LOG_DEBUG("Stopping...\n");

    // cleanup resources
    //mFloorsSensor.disconnect();

    // We must release message because this is the last event.
    mKernel.comm.releaseMessage(msg);

    // Waiting for the kernel to kill this app
    mKernel.sys.exit(0); // no return

}

void Service::onGlanceStart(SDK::MessageBase *msg)
{
    LOG_DEBUG("Glance start\n");
    //mFloorsSensor.connect();
}

void Service::onGlanceStop(SDK::MessageBase *msg)
{
    LOG_DEBUG("Glance stop\n");
    // cleanup resources
    //mFloorsSensor.disconnect();
}

void Service::onGlanceTick(SDK::MessageBase *msg)
{
    LOG_DEBUG("Glance tick\n");

    mGlanceValue.print("%u", mFloorsValue);

//    if (invalidate) {
//        auto* upd = mKernel.comm.allocateMessage<SDK::Message::RequestGlanceUpdate>();
//        if (upd) {
//            upd->name = mName;
//            upd->controls = mGlanceUI.data();
//            upd->controlsNumber = static_cast<uint32_t>(mGlanceUI.size());
//
//            mKernel.comm.sendMessage(upd, 100);
//            mKernel.comm.releaseMessage(upd);
//        }
//
//        invalidate = false;
//    }
//
//

}


//void Service::onSdlNewData(const SDK::Interface::ISensorDriver*              sensor,
//                           const std::vector< SDK::Interface::ISensorData*>& data,
//                           bool                                              first)
//{
//    // We are only interested in the last sample
//    const size_t sampleNum = data.size();
//    if (sampleNum == 0) {
//        return;
//    }
//
//    const SDK::Interface::ISensorData& sample = *data[sampleNum - 1];
//
//    if (mFloorsSensor.matchesDriver(sensor)) {
//        SDK::SensorDataParser::FloorCounter f {sample};
//        if (f.isDataValid()) {
//            mFloorsValue = f.getFloorsDown() + f.getFloorsUp();
//        }
//    }
//
//}

void Service::createGlanceGUI()
{
    mGlanceUI.createImage().init({20, 0}, {60, 60}, ICON_60X60_ABGR2222);

    mGlanceTitle = mGlanceUI.createText();
    mGlanceTitle.pos({ 70, 0 }, { 100, 25 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_20)
        .color(GlanceColor_t::GLANCE_COLOR_TEAL)
        .setText(mName)
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);

    mGlanceValue = mGlanceUI.createText();
    mGlanceValue.pos({ 80, 28 }, { 80, 34 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_30)
        .color(GlanceColor_t::GLANCE_COLOR_WHITE)
        .setText("")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);

}
