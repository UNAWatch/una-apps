#include "Service.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"
#include <array>
#include <memory>

#define LOG_MODULE_PRX "Service"
#define LOG_MODULE_LEVEL LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include "SDK/Messages/CommandMessages.hpp"
#include "SDK/Messages/SensorLayerMessages.hpp"
#include "SDK/SensorLayer/SensorDataBatch.hpp"

#include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRate.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserActivity.hpp"

#include "SDK/Interfaces/IFileSystem.hpp"

#include "icon_60x60.h"
#include "icon_30x30.h"

Service::Service(SDK::Kernel &kernel)
    : mKernel(kernel),
      mName("Strain Score"),
      mGlanceUI(),
      mGlanceTitle(),
      mGlanceValue(),
      mSensorHR(SDK::Sensor::Type::HEART_RATE),
      mSensorActivity(SDK::Sensor::Type::ACTIVITY) {}

Service::~Service() { disconnect(); }

void Service::run() {
    LOG_INFO("Started\n");

    createGuiControls();
    LOG_DEBUG("GUI controls created\n");
    if (configGui()) {
        LOG_DEBUG("GUI configured successfully\n");
        connect();
        checkDayRollover();
    } else {
        return;
    }

    while (true) {
        SDK::MessageBase *msg;

        if (!mKernel.comm.getMessage(msg)) {
            continue;
        }

        switch (msg->getType()) {
            case SDK::MessageType::EVENT_GLANCE_START:
                LOG_INFO("GLANCE is now running\n");

            case SDK::MessageType::COMMAND_APP_STOP:
                LOG_INFO("Force exit from the application\n");
            case SDK::MessageType::EVENT_GLANCE_STOP:
                LOG_INFO("GLANCE has stopped\n");
                saveJson();
                // disconnect();
                // mKernel.comm.releaseMessage(msg);
                // return;

            case SDK::MessageType::EVENT_GLANCE_TICK:
                LOG_DEBUG("Glance tick event\n");
                onGlanceTick();
                break;

            case SDK::MessageType::EVENT_SENSOR_LAYER_DATA: {
                LOG_DEBUG("Sensor data event\n");
                auto event = static_cast<SDK::Message::Sensor::EventData *>(msg);
                this->onSdlNewData(event->handle, event->data, event->count, event->stride);
            } break;

            default:
                break;
        }

        mKernel.comm.releaseMessage(msg);
    }
}

void Service::connect() {
    if (!mSensorHR.isConnected()) {
        LOG_DEBUG("Connect to HR sensors...\n");
        mSensorHR.connect();
    }
    if (!mSensorActivity.isConnected()) {
        LOG_DEBUG("Connect to Activity sensor...\n");
        mSensorActivity.connect();
    }
}

void Service::disconnect() {
    LOG_DEBUG("Disconnect from sensors...\n");
    saveJson();
    if (mSensorHR.isConnected()) {
        mSensorHR.disconnect();
    }
    if (mSensorActivity.isConnected()) {
        mSensorActivity.disconnect();
    }
}

void Service::onSdlNewData(uint16_t handle, const SDK::Sensor::Data *data, uint16_t count, uint16_t stride) {
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

void Service::onGlanceTick() {
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
        auto *upd = mKernel.comm.allocateMessage<SDK::Message::RequestGlanceUpdate>();
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

bool Service::configGui() {
    bool status = false;
    auto *gc = mKernel.comm.allocateMessage<SDK::Message::RequestGlanceConfig>();
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

void Service::createGuiControls() {
    mGlanceUI.createImage().init({31, 15}, {60, 60}, ICON_60X60_ABGR2222);

    mGlanceTitle = mGlanceUI.createText();
    mGlanceTitle.pos({20, 0}, {200, 25})
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_20)
        .color(GlanceColor_t::GLANCE_COLOR_TEAL)
        .setText("Strain Score")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);

    mGlanceValue = mGlanceUI.createText();
    mGlanceValue.pos({80, 28}, {80, 34})
        .font(GlanceFont_t::GLANCE_FONT_POPPINS_SEMIBOLD_30)
        .color(GlanceColor_t::GLANCE_COLOR_WHITE)
        .setText("")
        .alignment(GlanceAlignH_t::GLANCE_ALIGN_H_CENTER);
}

void Service::checkDayRollover() {
    LOG_DEBUG("Checking day rollover\n");
    std::time_t now_t = std::time(nullptr);
    std::tm tm_now;
    localtime_r(&now_t, &tm_now);
    char now_date[11];
    std::strftime(now_date, sizeof(now_date), "%Y-%m-%d", &tm_now);
    LOG_DEBUG("Current date: %s\n", now_date);

    if (std::strlen(mCurrentDate) == 0 || std::strcmp(mCurrentDate, now_date) != 0) {
        LOG_DEBUG("Date changed from '%s' to '%s'\n", mCurrentDate, now_date);
        if (std::strlen(mCurrentDate) > 0) {
            saveJson();
        }
        std::strncpy(mCurrentDate, now_date, 10);
        mCurrentDate[10] = '\0';
        std::snprintf(mJsonPath, sizeof(mJsonPath), "strain_%s.json", mCurrentDate);

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

void Service::saveJson() {
    LOG_DEBUG("Saving JSON to %s\n", mJsonPath);
    auto json_buf = std::make_shared<std::array<uint8_t, 65536>>();
    size_t offset = 0;
    float avg_hr = (mSampleCount > 0) ? (mSumHR / static_cast<float>(mSampleCount)) : 0.0f;

    offset = std::snprintf(
        reinterpret_cast<char *>(json_buf->data()), json_buf->size(),
        "{\"date\":\"%s\",\"total_strain\":%.1f,\"avg_hr\":%.1f,\"max_hr\":%u,\"active_min\":%lu,\"samples\":[",
        mCurrentDate, mTotalStrain, avg_hr, mMaxHR, mActiveMin);
    LOG_DEBUG("Initial JSON offset: %zu\n", offset);

    bool first_sample = true;
    for (const auto &s : mSamples) {
        if (!first_sample) {
            offset +=
                std::snprintf(reinterpret_cast<char *>(json_buf->data()) + offset, json_buf->size() - offset, ",");
        } else {
            first_sample = false;
        }
        offset += std::snprintf(reinterpret_cast<char *>(json_buf->data()) + offset, json_buf->size() - offset,
                                "{\"timestamp\":%llu,\"hr\":%hu,\"strain_delta\":%.3f}",
                                static_cast<unsigned long long>(s.timestamp), static_cast<unsigned short>(s.hr),
                                s.strain_delta);
    }
    LOG_DEBUG("Samples processed, offset: %zu\n", offset);
    offset += std::snprintf(reinterpret_cast<char *>(json_buf->data()) + offset, json_buf->size() - offset, "]}");
    LOG_DEBUG("Final JSON offset: %zu\n", offset);

    if (offset > 0 && offset < json_buf->size()) {
        // mKernel.fs.mkdir("/Apps");
        // mKernel.fs.mkdir("/Apps/strain");
        // mKernel.fs.mkdir("/Apps/strain/storage");
        auto file = mKernel.fs.file(mJsonPath);
        if (file && file->open(true, true)) {  // write mode, create/override
            size_t bw;
            file->write(reinterpret_cast<const char *>(json_buf->data()), offset, bw);
            if (bw == offset) {
                LOG_INFO("Saved strain data to %s\n", mJsonPath);
            } else {
                LOG_ERROR("Failed to write complete JSON data\n");
            }
            file->close();
        } else {
            LOG_ERROR("Failed to open file %s for writing\n", mJsonPath);
        }
    } else {
        LOG_ERROR("JSON buffer overflow or empty\n");
    }
}