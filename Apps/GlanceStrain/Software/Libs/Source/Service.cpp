#include "Service.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include "SDK/Messages/CommandMessages.hpp"
#include "SDK/Messages/SensorLayerMessages.hpp"
#include "SDK/SensorLayer/SensorDataBatch.hpp"

#include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRate.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserActivity.hpp"

#include "icon_60x60.h"
#include "icon_30x30.h"

Service::Service(SDK::Kernel &kernel)
    : mKernel(kernel)
    , mName("Strain Score")
    , mGlanceUI()
    , mGlanceTitle()
    , mGlanceValue()
    , mSensorHR(SDK::Sensor::Type::HEART_RATE)
    , mSensorActivity(SDK::Sensor::Type::ACTIVITY)
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

        if(!mKernel.comm.getMessage(msg)) {
            continue;
        }

        
        switch (msg->getType()) {
            case SDK::MessageType::EVENT_GLANCE_START:
            LOG_INFO("GLANCE is now running\n");
            if (configGui()) {
                    createGuiControls();
                    connect();
                    checkDayRollover();

                } else {
                    mKernel.comm.releaseMessage(msg);
                    mKernel.sys.exit(0);
                }
                break;

            case SDK::MessageType::COMMAND_APP_STOP:
                LOG_INFO("Force exit from the application\n");
            case SDK::MessageType::EVENT_GLANCE_STOP:
                LOG_INFO("GLANCE has stopped\n");
                // disconnect();
                // mKernel.comm.releaseMessage(msg);
                // return;

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

        mKernel.comm.releaseMessage(msg);
    }
}

void Service::connect()
{
    if (!mSensorHR.isConnected()) {
        LOG_DEBUG("Connect to HR sensors...\n");
        mSensorHR.connect();
    }
    if (!mSensorActivity.isConnected()) {
        LOG_DEBUG("Connect to Activity sensor...\n");
        mSensorActivity.connect();
    }
}

void Service::disconnect()
{
    LOG_DEBUG("Disconnect from sensors...\n");
    saveJson();
    if (mSensorHR.isConnected()) {
        mSensorHR.disconnect();
    }
    if (mSensorActivity.isConnected()) {
        mSensorActivity.disconnect();
    }
}

void Service::onSdlNewData(uint16_t handle,
                           const SDK::Sensor::Data* data,
                           uint16_t count,
                           uint16_t stride)
{
    SDK::Sensor::DataBatch batch(data, count, stride);

    if (mSensorActivity.matchesDriver(handle)) {
        if (count > 0) {
            SDK::SensorDataParser::Activity p(batch[0]);
            if (p.isDataValid()) {
                mActiveMin = p.getDuration();
            }
        }
    }

    if (mSensorHR.matchesDriver(handle)) {
        for (uint16_t i = 0; i < count; ++i) {
            SDK::SensorDataParser::HeartRate p(batch[i]);
            if (p.isDataValid()) {
                uint16_t hr = p.getBpm();
                if (hr >= 50 && hr <= 220) {
                    float norm = (static_cast<float>(hr) - 60.0f) / 120.0f;
                    float delta = std::max(0.0f, norm) * 0.75f;
                    mTotalStrain += delta;
                    mSumHR += static_cast<float>(hr);
                    mMaxHR = std::max(mMaxHR, hr);
                    mSampleCount++;
                    uint64_t ts = p.getTimestamp();
                    if (mSamples.size() < 1440u) {
                        mSamples.push_back({ts, hr, delta});
                    }
                }
            }
        }
    }
}

void Service::onGlanceTick()
{
    checkDayRollover();

    // float avg_hr = (mSampleCount > 0) ? (mSumHR / static_cast<float>(mSampleCount)) : 0.0f;

    mGlanceValue.print("%.1f", mTotalStrain);

    mTickCount++;

    std::time_t now = std::time(nullptr);
    bool need_save = false;
    if (mTickCount % 60 == 0) need_save = true;
    if (now - mLastSaveTime > 3600) need_save = true;
    if (need_save) {
        saveJson();
        mLastSaveTime = now;
    }

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
    bool status = false;
    auto* gc = mKernel.comm.allocateMessage<SDK::Message::RequestGlanceConfig>();
    if (gc) {
        if (mKernel.comm.sendMessage(gc, 100) && gc->getResult() == SDK::MessageResult::SUCCESS) {
            if (gc->maxControls >= 3) {
                mGlanceUI.setWidth(gc->width);
                mGlanceUI.setHeight(gc->height);
                status = true;
            }
        }
        mKernel.comm.releaseMessage(gc);
    }

    return status;
}

void Service::createGuiControls()
{
    mGlanceUI.createImage().init({31, 15}, {60, 60}, ICON_60X60_ABGR2222);

    mGlanceTitle = mGlanceUI.createText();
    mGlanceTitle.pos({ 20, 0 }, { 200, 25 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_20)
        .color(GlanceColor_t::GLANCE_COLOR_TEAL)
        .setText("Strain Score")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);

    mGlanceValue = mGlanceUI.createText();
    mGlanceValue.pos({ 80, 28 }, { 80, 34 })
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_30)
        .color(GlanceColor_t::GLANCE_COLOR_WHITE)
        .setText("")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);
}

void Service::checkDayRollover()
{
    std::time_t now_t = std::time(nullptr);
    std::tm tm_now;
    localtime_r(&now_t, &tm_now);
    char now_date[11];
    std::strftime(now_date, sizeof(now_date), "%Y-%m-%d", &tm_now);

    if (std::strlen(mCurrentDate) == 0 || std::strcmp(mCurrentDate, now_date) != 0) {
        if (std::strlen(mCurrentDate) > 0) {
            saveJson();
        }
        std::strncpy(mCurrentDate, now_date, 10);
        mCurrentDate[10] = '\0';
        std::snprintf(mJsonPath, sizeof(mJsonPath), "/storage/strain_%s.json", mCurrentDate);

        mTotalStrain = 0.0f;
        mSumHR = 0.0f;
        mMaxHR = 0;
        mActiveMin = 0;
        mSampleCount = 0;
        mSamples.clear();
        mTickCount = 0;
        mLastSaveTime = now_t;

        LOG_INFO("New strain day: %s\\n", mCurrentDate);
    }
}

void Service::saveJson()
{
    return ;
    // char json_buf[65536] = {0};
    // size_t offset = 0;
    // float avg_hr = (mSampleCount > 0) ? (mSumHR / static_cast<float>(mSampleCount)) : 0.0f;

    // offset = std::snprintf(json_buf, sizeof(json_buf),
    //     "{\"date\":\"%s\",\"total_strain\":%.1f,\"avg_hr\":%.1f,\"max_hr\":%u,\"active_min\":%lu,\"samples\":[",
    //     mCurrentDate, mTotalStrain, avg_hr, mMaxHR, mActiveMin);

    // bool first_sample = true;
    // for (const auto& s : mSamples) {
    //     if (!first_sample) {
    //         offset += std::snprintf(json_buf + offset, sizeof(json_buf) - offset, ",");
    //     } else {
    //         first_sample = false;
    //     }
    //     offset += std::snprintf(json_buf + offset, sizeof(json_buf) - offset,
    //         "{\"timestamp\":%llu,\"hr\":%hu,\"strain_delta\":%.3f}",
    //         static_cast<unsigned long long>(s.timestamp), static_cast<unsigned short>(s.hr), s.strain_delta);
    // }
    // offset += std::snprintf(json_buf + offset, sizeof(json_buf) - offset, "]}");

    // if (offset > 0 && offset < sizeof(json_buf)) {
    //     LOG_INFO("Strain JSON: %s\n", json_buf);
    //     // TODO: Implement file saving using SDK storage API
    // }
}