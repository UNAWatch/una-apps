#include "Service.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"
#include "SDK/Tools/FirmwareVersion.hpp"
#include <array>
#include <memory>
#include <cstring>
#include <algorithm>

#define LOG_MODULE_PRX "Service"
#define LOG_MODULE_LEVEL LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include "SDK/Messages/CommandMessages.hpp"
#include "SDK/Messages/SensorLayerMessages.hpp"
#include "SDK/SensorLayer/SensorDataBatch.hpp"

#include "SDK/SensorLayer/DataParsers/SensorDataParserHeartRate.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserActivity.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserTouch.hpp"

#include "SDK/Interfaces/IFileSystem.hpp"

extern "C" {
#include "fit_product.h"
#include "fit_crc.h"
}

#ifndef DEV_ID
#define DEV_ID "UNA"
#endif

#ifndef APP_ID
#define APP_ID "STRAIN"
#endif

namespace {
constexpr std::time_t kFitEpochOffset = 631065600;

static std::time_t tm2epoch(const std::tm* tm) {
    int y = tm->tm_year + 1900;
    int m = tm->tm_mon + 1;
    int d = tm->tm_mday;

    if (m <= 2) {
        y -= 1;
        m += 12;
    }

    int64_t era = (y >= 0 ? y : y - 399) / 400;
    uint32_t yoe = static_cast<uint32_t>(y - era * 400);
    uint32_t doy = (153 * (m - 3) + 2) / 5 + d - 1;
    uint32_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    int64_t days = era * 146097 + static_cast<int64_t>(doe) - 719468;
    int64_t secs = days * 86400 + tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;

    return static_cast<std::time_t>(secs);
}

static std::time_t epochToLocal(std::time_t utc) {
    std::tm localTime{};
#if WIN32
    localtime_s(&localTime, &utc);
#else
    localtime_r(&utc, &localTime);
#endif
    return tm2epoch(&localTime);
}

static FIT_DATE_TIME unixToFitTimestamp(std::time_t unixTimestamp) {
    return static_cast<FIT_DATE_TIME>(unixTimestamp - kFitEpochOffset);
}

static void writeFileHeader(SDK::Interface::IFile* fp) {
    FIT_FILE_HDR file_header{};

    file_header.header_size = FIT_FILE_HDR_SIZE;
    file_header.profile_version = FIT_PROFILE_VERSION;
    file_header.protocol_version = FIT_PROTOCOL_VERSION_20;
    std::memcpy(reinterpret_cast<FIT_UINT8*>(&file_header.data_type), ".FIT", 4);

    fp->flush();
    size_t fileSize = fp->size();

    if (fileSize > FIT_FILE_HDR_SIZE) {
        file_header.data_size = static_cast<FIT_UINT32>(fileSize - FIT_FILE_HDR_SIZE);
    } else {
        file_header.data_size = 0;
    }

    file_header.crc = FitCRC_Calc16(&file_header, FIT_STRUCT_OFFSET(crc, FIT_FILE_HDR));

    fp->seek(0);

    size_t bw;
    fp->write(reinterpret_cast<const char*>(&file_header), FIT_FILE_HDR_SIZE, bw);
    fp->flush();

    if (fileSize > 0) {
        fp->seek(fileSize);
    }
}

static void writeCRC(SDK::Interface::IFile* fp) {
    fp->close();
    fp->open(false);

    FIT_UINT8 buffer[512];
    size_t size = fp->size();
    size_t pos = 0;
    uint16_t crc = 0;

    while (pos < size) {
        size_t toRead = size - pos;
        if (toRead > sizeof(buffer)) {
            toRead = sizeof(buffer);
        }

        size_t br;
        fp->read(reinterpret_cast<char*>(buffer), toRead, br);
        crc = FitCRC_Update16(crc, buffer, static_cast<FIT_UINT32>(br));
        pos += br;
    }

    fp->close();
    fp->open(true, false);
    fp->seek(fp->size());

    size_t bw;
    fp->write(reinterpret_cast<const char*>(&crc), sizeof(FIT_UINT16), bw);
    fp->flush();
}
}  // namespace

#include "icon_60x60.h"
#include "icon_30x30.h"

Service::Service(SDK::Kernel& kernel)
    : mKernel(kernel),
      mName("Strain Score"),
      mGlanceUI(),
      mGlanceTitle(),
      mGlanceValue(),
      mSensorHR(SDK::Sensor::Type::HEART_RATE),
      mSensorActivity(SDK::Sensor::Type::ACTIVITY),
      mSensorTouch(SDK::Sensor::Type::TOUCH_DETECT),
      mFitFileID(skFileMsgNum, (FIT_MESG_DEF*)fit_mesg_defs[FIT_MESG_FILE_ID]),
      mFitDeveloper(skDevelopMsgNum, (FIT_MESG_DEF*)fit_mesg_defs[FIT_MESG_DEVELOPER_DATA_ID]),
      mFitRecord(skRecordMsgNum, (FIT_MESG_DEF*)fit_mesg_defs[FIT_MESG_RECORD]),
      mFitEvent(skEventMsgNum, (FIT_MESG_DEF*)fit_mesg_defs[FIT_MESG_EVENT]),
      mFitSession(skSessionMsgNum, (FIT_MESG_DEF*)fit_mesg_defs[FIT_MESG_SESSION]),
      mFitActivity(skActivityMsgNum, (FIT_MESG_DEF*)fit_mesg_defs[FIT_MESG_ACTIVITY]),
      mFitStrainField(skStrainMsgNum, {&mFitRecord}),
      mFitActiveField(skActiveMsgNum, {&mFitRecord}) {
    mFitFileID.init();
    mFitDeveloper.init();
    mFitEvent.init({FIT_EVENT_FIELD_NUM_TIMESTAMP, FIT_EVENT_FIELD_NUM_EVENT, FIT_EVENT_FIELD_NUM_EVENT_TYPE});
    mFitActivity.init({FIT_ACTIVITY_FIELD_NUM_TIMESTAMP, FIT_ACTIVITY_FIELD_NUM_TOTAL_TIMER_TIME,
                       FIT_ACTIVITY_FIELD_NUM_LOCAL_TIMESTAMP, FIT_ACTIVITY_FIELD_NUM_NUM_SESSIONS});
    mFitRecord.init({FIT_RECORD_FIELD_NUM_TIMESTAMP, FIT_RECORD_FIELD_NUM_HEART_RATE});
    mFitSession.init({FIT_SESSION_FIELD_NUM_TIMESTAMP, FIT_SESSION_FIELD_NUM_START_TIME,
                      FIT_SESSION_FIELD_NUM_TOTAL_ELAPSED_TIME, FIT_SESSION_FIELD_NUM_TOTAL_TIMER_TIME,
                      FIT_SESSION_FIELD_NUM_MESSAGE_INDEX, FIT_SESSION_FIELD_NUM_SPORT, FIT_SESSION_FIELD_NUM_SUB_SPORT,
                      FIT_SESSION_FIELD_NUM_AVG_HEART_RATE, FIT_SESSION_FIELD_NUM_MAX_HEART_RATE});
    mFitStrainField.init({FIT_FIELD_DESCRIPTION_FIELD_NUM_FIELD_NAME, FIT_FIELD_DESCRIPTION_FIELD_NUM_UNITS,
                          FIT_FIELD_DESCRIPTION_FIELD_NUM_DEVELOPER_DATA_INDEX,
                          FIT_FIELD_DESCRIPTION_FIELD_NUM_FIELD_DEFINITION_NUMBER,
                          FIT_FIELD_DESCRIPTION_FIELD_NUM_FIT_BASE_TYPE_ID});
    mFitActiveField.init({FIT_FIELD_DESCRIPTION_FIELD_NUM_FIELD_NAME, FIT_FIELD_DESCRIPTION_FIELD_NUM_UNITS,
                          FIT_FIELD_DESCRIPTION_FIELD_NUM_DEVELOPER_DATA_INDEX,
                          FIT_FIELD_DESCRIPTION_FIELD_NUM_FIELD_DEFINITION_NUMBER,
                          FIT_FIELD_DESCRIPTION_FIELD_NUM_FIT_BASE_TYPE_ID});
}

Service::~Service() {}

void Service::run() {
    LOG_INFO("Started\n");

    if (!configGui()) {
        LOG_ERROR("Can't create glance-gui\n");
        return;
    }
    createGuiControls();
    connect();

    while (true) {
        SDK::MessageBase* msg = {nullptr};
        if (!mKernel.comm.getMessage(msg)) {
            continue;
        }

        switch (msg->getType()) {
            case SDK::MessageType::EVENT_GLANCE_START:
                LOG_INFO("GLANCE is now running\n");
                mGlanceActive = true;
                break;

            case SDK::MessageType::EVENT_GLANCE_STOP:
                LOG_INFO("GLANCE has stopped\n");
                mGlanceActive = false;
                break;

            case SDK::MessageType::COMMAND_APP_STOP:
                LOG_INFO("Force exit from the application\n");
                disconnect();
                mKernel.comm.releaseMessage(msg);
                return;

            case SDK::MessageType::EVENT_GLANCE_TICK:
                LOG_DEBUG("Glance tick event\n");
                onGlanceTick();
                break;

            case SDK::MessageType::EVENT_SENSOR_LAYER_DATA: {
                LOG_DEBUG("Sensor data event\n");
                auto event = static_cast<SDK::Message::Sensor::EventData*>(msg);
                this->onSdlNewData(event->handle, event->data, event->count, event->stride);
            } break;

            default:
                break;
        }

        mKernel.comm.releaseMessage(msg);
    }
}

void Service::connect() {
    const float samplePeriodMs = static_cast<float>(skSamplePeriodSec) * 1000.0f;
    if (!mSensorHR.isConnected()) {
        LOG_DEBUG("Connect to HR sensors...\n");
        mSensorHR.connect(samplePeriodMs);
    }
    if (!mSensorActivity.isConnected()) {
        LOG_DEBUG("Connect to Activity sensor...\n");
        mSensorActivity.connect(samplePeriodMs);
    }
    if (!mSensorTouch.isConnected()) {
        LOG_DEBUG("Connect to Touch sensor...\n");
        mSensorTouch.connect(samplePeriodMs);
    }
}

void Service::disconnect() {
    LOG_DEBUG("Disconnect from sensors...\n");
    saveFit(true, true);
    if (mSensorHR.isConnected()) {
        mSensorHR.disconnect();
    }
    if (mSensorActivity.isConnected()) {
        mSensorActivity.disconnect();
    }
    if (mSensorTouch.isConnected()) {
        mSensorTouch.disconnect();
    }
}

void Service::onSdlNewData(uint16_t handle, const SDK::Sensor::Data* data, uint16_t count, uint16_t stride) {
    std::time_t now = std::time(nullptr);
    checkDayRollover();

    SDK::Sensor::DataBatch batch(data, count, stride);

    if (mSensorTouch.matchesDriver(handle)) {
        if (count > 0) {
            SDK::SensorDataParser::Touch p(batch[0]);
            if (p.isDataValid()) {
                bool onHand = p.isTouched();
                if (mIsOnHand != onHand) {
                    mIsOnHand = onHand;
                    if (!mIsOnHand) {
                        /** @note: save data on watch hand-off */
                        LOG_INFO("Watch hand off, saving data\n");
                        saveFit(true, false);
                    } else {
                        LOG_INFO("Watch is worn on the wrist\n");
                    }
                }
            }
        }
    }

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
            if (!p.isDataValid() || !mIsOnHand) continue;
            float hrRaw = p.getBpm();
            LOG_DEBUG("HR: %f\n", hrRaw);
            uint16_t hr = static_cast<uint16_t>(hrRaw);
            if (hr >= 50 && hr <= 255) {
                float norm = (static_cast<float>(hr) - 60.0f) / 120.0f;
                float delta = std::max(0.0f, norm) * 0.75f;
                mTotalStrain += delta;
                mSumHR += static_cast<float>(hr);
                mMaxHR = std::max(mMaxHR, hr);
                mSampleCount++;
                mLastHr = hr;
                if (mIsOnHand && mSampleCount > 0) {
                    mPendingRecords.push_back(
                        {now, static_cast<uint8_t>(std::min<uint16_t>(mLastHr, 255)), mTotalStrain, mActiveMin});
                }
            }
        }
    }

    if ((now - mLastSaveTime) >= skSaveIntervalSec) {
        saveFit(false, false);
    }
}

void Service::onGlanceTick() {
    mGlanceValue.print("%.1f", mTotalStrain);

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

bool Service::configGui() {
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
    std::time_t now_t = std::time(nullptr);
    std::tm tm_now;
    localtime_r(&now_t, &tm_now);
    char now_date[11];
    std::strftime(now_date, sizeof(now_date), "%Y-%m-%d", &tm_now);

    if (std::strlen(mCurrentDate) == 0 || std::strcmp(mCurrentDate, now_date) != 0) {
        LOG_DEBUG("Date changed from '%s' to '%s'\n", mCurrentDate, now_date);
        if (std::strlen(mCurrentDate) > 0) {
            saveFit(true, true);
        }
        std::strncpy(mCurrentDate, now_date, 10);
        mCurrentDate[10] = '\0';
        std::snprintf(mFitPath, sizeof(mFitPath), "strain_%s.fit", mCurrentDate);

        mTotalStrain = 0.0f;
        mSumHR = 0.0f;
        mMaxHR = 0;
        mActiveMin = 0;
        mSampleCount = 0;
        mPendingRecords.clear();
        mLastSaveTime = 0;
        mDayStart = now_t;
        mFitFileInitialized = false;

        LOG_INFO("New day: %s\\n", mCurrentDate);
    }
}

void Service::writeFitDefinitions(SDK::Interface::IFile* fp, std::time_t timestamp) {
    writeFileHeader(fp);

    mFitFileID.writeDef(fp);
    FIT_FILE_ID_MESG file_id_mesg{};
    std::strncpy(file_id_mesg.product_name, "UNA Watch", FIT_FILE_ID_MESG_PRODUCT_NAME_COUNT);
    file_id_mesg.serial_number = 0;
    file_id_mesg.time_created = unixToFitTimestamp(timestamp);
    file_id_mesg.manufacturer = FIT_MANUFACTURER_DEVELOPMENT;
    file_id_mesg.product = 0;
    file_id_mesg.number = 0;
    file_id_mesg.type = FIT_FILE_ACTIVITY;
    mFitFileID.writeMessage(&file_id_mesg, fp);

    mFitDeveloper.writeDef(fp);
    FIT_DEVELOPER_DATA_ID_MESG developer{};
    std::strncpy(reinterpret_cast<char*>(developer.developer_id), DEV_ID,
                 FIT_DEVELOPER_DATA_ID_MESG_DEVELOPER_ID_COUNT);
    std::strncpy(reinterpret_cast<char*>(developer.application_id), APP_ID,
                 FIT_DEVELOPER_DATA_ID_MESG_APPLICATION_ID_COUNT);
    developer.application_version = SDK::ParseVersion(BUILD_VERSION).u32;
    developer.manufacturer_id = FIT_MANUFACTURER_DEVELOPMENT;
    developer.developer_data_index = 0;
    mFitDeveloper.writeMessage(&developer, fp);

    mFitEvent.writeDef(fp);
    mFitActivity.writeDef(fp);
    mFitRecord.writeDef(fp);
    mFitSession.writeDef(fp);

    mFitStrainField.writeDef(fp);
    FIT_FIELD_DESCRIPTION_MESG strainField{};
    std::strncpy(strainField.field_name, "strain", FIT_FIELD_DESCRIPTION_MESG_FIELD_NAME_COUNT);
    std::strncpy(strainField.units, "score", FIT_FIELD_DESCRIPTION_MESG_UNITS_COUNT);
    strainField.developer_data_index = 0;
    strainField.field_definition_number = 0;
    strainField.fit_base_type_id = FIT_BASE_TYPE_FLOAT32;
    mFitStrainField.writeMessage(&strainField, fp);

    mFitActiveField.writeDef(fp);
    FIT_FIELD_DESCRIPTION_MESG activeField{};
    std::strncpy(activeField.field_name, "active_min", FIT_FIELD_DESCRIPTION_MESG_FIELD_NAME_COUNT);
    std::strncpy(activeField.units, "minutes", FIT_FIELD_DESCRIPTION_MESG_UNITS_COUNT);
    activeField.developer_data_index = 0;
    activeField.field_definition_number = 1;
    activeField.fit_base_type_id = FIT_BASE_TYPE_UINT32;
    mFitActiveField.writeMessage(&activeField, fp);

    FIT_EVENT_MESG start_event{};
    start_event.timestamp = unixToFitTimestamp(timestamp);
    start_event.event = FIT_EVENT_TIMER;
    start_event.event_type = FIT_EVENT_TYPE_START;
    mFitEvent.writeMessage(&start_event, fp);
}

void Service::appendPendingRecords(SDK::Interface::IFile* fp) {
    if (mPendingRecords.empty()) {
        return;
    }

    for (const auto& rec : mPendingRecords) {
        FIT_RECORD_MESG record_mesg{};
        record_mesg.timestamp = unixToFitTimestamp(rec.timestamp);
        record_mesg.heart_rate = rec.hr;
        mFitRecord.writeMessage(&record_mesg, fp);

        float strain = rec.strain_delta;
        mFitRecord.writeFieldMessage(0, &strain, fp);

        FIT_UINT32 active = rec.active_min;
        mFitRecord.writeFieldMessage(1, &active, fp);
    }

    mPendingRecords.clear();
}

void Service::writeFitSessionSummary(SDK::Interface::IFile* fp, std::time_t timestamp) {
    float avg_hr = (mSampleCount > 0) ? (mSumHR / static_cast<float>(mSampleCount)) : 0.0f;

    FIT_EVENT_MESG stop_event{};
    stop_event.timestamp = unixToFitTimestamp(timestamp);
    stop_event.event = FIT_EVENT_TIMER;
    stop_event.event_type = FIT_EVENT_TYPE_STOP;
    mFitEvent.writeMessage(&stop_event, fp);

    FIT_SESSION_MESG session_mesg{};
    session_mesg.message_index = 0;
    session_mesg.sport = FIT_SPORT_GENERIC;
    session_mesg.sub_sport = FIT_SUB_SPORT_GENERIC;
    session_mesg.timestamp = unixToFitTimestamp(timestamp);
    session_mesg.start_time = unixToFitTimestamp(mDayStart);
    session_mesg.total_elapsed_time = static_cast<FIT_UINT32>((timestamp - mDayStart) * 1000);
    session_mesg.total_timer_time = static_cast<FIT_UINT32>((timestamp - mDayStart) * 1000);
    session_mesg.avg_heart_rate = static_cast<FIT_UINT8>(avg_hr);
    session_mesg.max_heart_rate = static_cast<FIT_UINT8>(mMaxHR);
    mFitSession.writeMessage(&session_mesg, fp);

    FIT_ACTIVITY_MESG activity_mesg{};
    activity_mesg.timestamp = unixToFitTimestamp(timestamp);
    activity_mesg.local_timestamp = unixToFitTimestamp(epochToLocal(timestamp));
    activity_mesg.total_timer_time = static_cast<FIT_UINT32>((timestamp - mDayStart) * 1000);
    activity_mesg.num_sessions = 1;
    mFitActivity.writeMessage(&activity_mesg, fp);
}

void Service::saveFit(bool force, bool finalizeDay) {
    std::time_t now = std::time(nullptr);

    LOG_DEBUG("saveFit start (force=%d, finalizeDay=%d, onHand=%d)\n", force, finalizeDay, mIsOnHand);

    if (!force && !mIsOnHand) {
        LOG_DEBUG("saveFit skipped: watch not on hand\n");
        return;
    }

    auto file = mKernel.fs.file(mFitPath);
    if (!file) {
        LOG_ERROR("saveFit failed: can't access FIT path %s\n", mFitPath);
        return;
    }

    bool isNewFile = !file->exist();
    LOG_DEBUG("saveFit opening file %s (new=%d)\n", mFitPath, isNewFile);
    if (!file->open(true, isNewFile)) {
        LOG_ERROR("Failed to open FIT file %s\n", mFitPath);
        return;
    }

    if (!isNewFile) {
        size_t fileSize = file->size();
        LOG_DEBUG("saveFit existing file size=%zu\n", fileSize);
        if (fileSize >= (FIT_FILE_HDR_SIZE + sizeof(FIT_UINT16))) {
            file->truncate(fileSize - sizeof(FIT_UINT16));
            LOG_DEBUG("saveFit truncated CRC, new size=%zu\n", file->size());
        }

        if (file->size() > FIT_FILE_HDR_SIZE) {
            mFitFileInitialized = true;
        }

        file->seek(file->size());
    }

    if (isNewFile || !mFitFileInitialized) {
        LOG_DEBUG("saveFit writing FIT definitions\n");
        writeFitDefinitions(file.get(), now);
        mFitFileInitialized = true;
    }

    LOG_DEBUG("saveFit appending %zu pending records\n", mPendingRecords.size());
    size_t sizeBeforeAppend = file->size();
    LOG_DEBUG("saveFit size before append=%zu\n", sizeBeforeAppend);
    appendPendingRecords(file.get());
    size_t sizeAfterAppend = file->size();
    LOG_DEBUG("saveFit size after append=%zu\n", sizeAfterAppend);

    if (finalizeDay) {
        size_t sizeBeforeSummary = file->size();
        LOG_DEBUG("saveFit size before summary=%zu\n", sizeBeforeSummary);
        LOG_DEBUG("saveFit writing session summary\n");
        writeFitSessionSummary(file.get(), now);
        size_t sizeAfterSummary = file->size();
        LOG_DEBUG("saveFit size after summary=%zu\n", sizeAfterSummary);
    }

    file->flush();
    writeFileHeader(file.get());
    writeCRC(file.get());
    file->close();

    mLastSaveTime = now;
    LOG_INFO("saveFit complete: %s\n", mFitPath);
}
