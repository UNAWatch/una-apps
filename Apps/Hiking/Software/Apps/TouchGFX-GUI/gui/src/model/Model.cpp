#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <gui/common/FrontendApplication.hpp>

#include "SDK/Kernel/KernelProviderGUI.hpp"
#include "SDK/../../../Port/TouchGFX/TouchGFXCommandProcessor.hpp"

#define LOG_MODULE_PRX      "Model"
#define LOG_MODULE_LEVEL    LOG_LEVEL_INFO
#include "SDK/UnaLogger/Logger.h"

#if defined(SIMULATOR)
    #include "touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp"
    #include "Windows.h"
    #include <chrono>
    #include <ctime>
#endif

Model::Model()
    : modelListener(0)
    , mKernel(SDK::KernelProviderGUI::GetInstance().getKernel())
    , mSrvSender(mKernel)
{
    SDK::TouchGFXCommandProcessor::GetInstance().setAppLifeCycleCallback(this);
    SDK::TouchGFXCommandProcessor::GetInstance().setCustomMessageHandler(this);

    setCapabilities();

#if defined(SIMULATOR)
    LOG_INFO("Application is running through simulator! \n");

    std::string fileStoreDir = SDK::Simulator::KernelHolder::Get().getFsPath();
    LOG_INFO("Path to files created by app:\n"
        "       [%s]\n", fileStoreDir.c_str());

    LOG_INFO("\n"
        "       Keys:                       \n"
        "       ----------------------------\n"
        "       1   L1,                     \n"
        "       2   L2,                     \n"
        "       3   R1,                     \n"
        "       4   R2,                     \n"
        "       z   L1+R2                   \n"
    );
#endif
}

// Controls
FrontendApplication& Model::application()
{
    return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
}

void Model::invalidate()
{
    mInvalidate = true;
}


void Model::tick()
{
    if (mIsRunning) {
        decIdleTimer();
    }

    if (mInvalidate) {
        mInvalidate = false;
        application().invalidate();
    }
}

void Model::handleKeyEvent(uint8_t key)
{
    LOG_DEBUG("key = %c\n", static_cast<char>(key));

    if (isAnyKeyPressed(key)) {

        // Reset idle timer on any key press
        resetIdleTimer();
    }
}

void Model::resetIdleTimer()
{
    mIdleTimer = Gui::Config::kScreenTimeoutSteps;
}

void Model::exitApp()
{
    LOG_INFO("Manually exiting the application\n");
    // Cleanup recourses

    SDK::TouchGFXCommandProcessor::GetInstance().setAppLifeCycleCallback(nullptr);
    SDK::TouchGFXCommandProcessor::GetInstance().setCustomMessageHandler(nullptr);

    mKernel.sys.exit(); // No return for real app

    // !!! For TouchGFX Simulator !!!
    // This function only sets a flag. 
    // The current TouchGFX loop will be completed, meaning that depending 
    // on where this function was called, Model::tick(), Model::handleKeyEvent(), 
    // as well as handleTickEvent() and handleKeyEvent() for the 
    // current screen will be called.
}


// Menu position
void Model::setMenuPosEnterMenu(uint16_t p)
{
    mMenuPosition.enter = p;
}

uint16_t Model::getMenuPosEnterMenu()
{
    return mMenuPosition.enter;
}

void Model::setMenuPosTrack(uint16_t p)
{
    mMenuPosition.track = p;
}

uint16_t Model::getMenuPosTrack()
{
    return mMenuPosition.track;
}

void Model::setMenuPosTrackAction(uint16_t p)
{
    mMenuPosition.action = p;
}

uint16_t Model::getMenuPosTrackAction()
{
    return mMenuPosition.action;
}

void Model::setMenuPosMenuSettings(uint16_t p)
{
    mMenuPosition.settings = p;
}

uint16_t Model::getMenuPosMenuSettings()
{
    return mMenuPosition.settings;
}

void Model::setMenuPosMenuAlerts(uint16_t p)
{
    mMenuPosition.alerts = p;
}

uint16_t Model::getMenuPosMenuAlerts()
{
    return mMenuPosition.alerts;
}


// Date/Time
void Model::getDate(uint8_t& m, uint8_t& d, uint8_t& wd)
{
    std::tm tm = getDateTime();

    m = tm.tm_mon;
    d = tm.tm_mday;
    wd = tm.tm_wday;
}

void Model::getTime(uint8_t& h, uint8_t& m, uint8_t& s)
{
    std::tm tm = getDateTime();

    h = tm.tm_hour;
    m = tm.tm_min;
    s = tm.tm_sec;
}

std::tm Model::getDateTime()
{
    return mTime;
}


// Power
uint8_t Model::getBatteryLevel()
{
    return mBatteryLevel;
}


// Settings
bool Model::isUnitsImperial()
{
    return mUnitsImperial;
}

const std::array<uint8_t, 4>& Model::getHrThresholds() const
{
    return mHrThresholds;
}

const Settings& Model::getSettings() const
{
    return mSettings;
}

void Model::setSettings(const Settings& sett)
{
    mSettings = sett;
    mSrvSender.settingsSave(mSettings);
}


// GPS
bool Model::getGpsFix()
{
    return mGpsFix;
}


// Track
void Model::trackStart()
{
    mSrvSender.trackStart();
}

bool Model::trackIsActive()
{
    return mTrackState != Track::State::INACTIVE;
}

bool Model::trackIsPaused()
{
    return mTrackState == Track::State::PAUSED;
}

const Track::Data& Model::getTrackData() const
{
    return mTrackData;
}

void Model::saveTrack()
{
    mSrvSender.trackStop(false);
}

void Model::discardTrack()
{
    mSrvSender.trackStop(true);
}

bool Model::trackIsSummaryAvailable()
{
    return mpActivitySummary != nullptr && mpActivitySummary->time != 0;
}

const ActivitySummary& Model::trackSummary()
{
    return *mpActivitySummary;
}


// Protected
void Model::decIdleTimer()
{
    if (mIdleTimer > 0) {
        mIdleTimer--;
        if (mIdleTimer == 0) {
            modelListener->onIdleTimeout();
        }
    }
}

bool Model::isAnyKeyPressed(uint8_t key) const
{
    return (Gui::Config::Button::L1 == key) ||
        (Gui::Config::Button::L2 == key) ||
        (Gui::Config::Button::R1 == key) ||
        (Gui::Config::Button::R2 == key);
}

void Model::setCapabilities()
{
    auto *msg = mKernel.comm.allocateMessage<SDK::Message::RequestSetCapabilities>();
    if (msg) {
        msg->enMusicControl = true;
        msg->enUsbChargingScreen = false;
        msg->enMusicControl = true;
        mKernel.comm.sendMessage(msg);
        mKernel.comm.releaseMessage(msg);
    }
}

// IUserApp implementation

void Model::onStart()
{
    LOG_INFO("Started\n");
}

void Model::onResume()
{
    mIsRunning = true;
    resetIdleTimer();

    invalidate();   // Redraw screen
}

void Model::onSuspend()
{
    mIsRunning = false;
}

void Model::onStop()
{
    LOG_INFO("Force exit from the application\n");
}

// Events from Service

bool Model::customMessageHandler(SDK::MessageBase *msg)
{
    switch (msg->getType()) {
        case CustomMessage::SETTINGS_UPDATE:  {
            LOG_DEBUG("SETTINGS_UPDATE\n");
            auto *cmsg = static_cast<CustomMessage::SettingsUpd*>(msg);
            mSettings = cmsg->settings;
            mUnitsImperial = cmsg->unitsImperial;
        } break;

        case CustomMessage::LOCAL_TIME:  {
            auto *cmsg = static_cast<CustomMessage::Time*>(msg);

            std::tm newTime = cmsg->localTime;
            LOG_DEBUG("LOCAL_TIME %04u-%02u-%02u %02u:%02u:%02u\n",
                    newTime.tm_year + 1900, newTime.tm_mon + 1, newTime.tm_mday,
                    newTime.tm_hour, newTime.tm_min, newTime.tm_sec);

            bool dateChanged = (newTime.tm_year != mTime.tm_year ||
                newTime.tm_mon != mTime.tm_mon || newTime.tm_mday != mTime.tm_mday);

            bool timeChanged = (newTime.tm_hour != mTime.tm_hour ||
                                newTime.tm_min != mTime.tm_min ||
                                newTime.tm_sec != mTime.tm_sec);

            mTime = newTime;

            if (dateChanged) {
                modelListener->onDate(mTime.tm_year + 1900, mTime.tm_mon + 1, mTime.tm_mday, mTime.tm_wday);
            }

            if (timeChanged) {
                modelListener->onTime(mTime.tm_hour, mTime.tm_min, mTime.tm_sec);
            }
        } break;

        case CustomMessage::BATTERY:  {
            auto *cmsg = static_cast<CustomMessage::Battery*>(msg);
            LOG_DEBUG("BATTERY Level %u\n", cmsg->level);
            if (mBatteryLevel != cmsg->level) {
                mBatteryLevel = cmsg->level;
                modelListener->onBatteryLevel(mBatteryLevel);
            }
        } break;

        case CustomMessage::GPS_FIX:  {
            auto *cmsg = static_cast<CustomMessage::GpsFix*>(msg);
            LOG_DEBUG("GPS_FIX %u\n", cmsg->state);
            if (mGpsFix != cmsg->state) {
                mGpsFix = cmsg->state;
                modelListener->onGpsFix(mGpsFix);
            }
        } break;

        case CustomMessage::TRACK_STATE_UPDATE:  {
            LOG_DEBUG("TRACK_STATE_UPDATE\n");
            auto *cmsg = static_cast<CustomMessage::TrackStateUpd*>(msg);
            if (mTrackState != cmsg->state) {
                mTrackState = cmsg->state;
                modelListener->onTrackState(mTrackState);
            }
        } break;

        case CustomMessage::TRACK_DATA_UPDATE:  {
            LOG_DEBUG("TRACK_DATA_UPDATE\n");
            auto *cmsg = static_cast<CustomMessage::TrackDataUpd*>(msg);
            mTrackData = cmsg->data;
            modelListener->onTrackData(mTrackData);
        } break;

        case CustomMessage::LAP_END:  {
            LOG_DEBUG("LAP_END\n");
            auto *cmsg = static_cast<CustomMessage::LapEnded*>(msg);
            modelListener->onLapChanged(cmsg->lapNum);
        } break;

        case CustomMessage::SUMMARY:  {
            LOG_DEBUG("SUMMARY\n");
            auto *cmsg = static_cast<CustomMessage::Summary*>(msg);
            mpActivitySummary = cmsg->summary;
            modelListener->onActivitySummary(*mpActivitySummary);
        } break;

        default:
            break;
    }

    return true;
}
