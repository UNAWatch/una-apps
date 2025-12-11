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

FrontendApplication& Model::application()
{
    return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
}

// Model tick, called from GUI thread every frame 
// After this function returns, handleTickEvent() 
// and (if key) handleKeyEvent() called for current screen
void Model::tick()
{
    //LOG_DEBUG("tick\n");

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

        // If user presses any key while alarm is inactive (walking thru menu), 
        // stay in app
        if (!mActiveAlarm.on) {
            mStayInApp = true;
        }
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

void Model::switchToNextPriorityScreen()
{
    if (mActiveAlarm.on) {
        application().gotoAlarmScreenNoTransition();
        return;
    }

    if (mStayInApp) {
        mStayInApp = false;
        application().gotoMainScreenNoTransition();
        return; 
    }

    // Nothing to do, exit GUI app
    exitApp();
}

const AppType::Alarm& Model::getActiveAlarm() const
{
    return mActiveAlarm;
}

void Model::playAlarm()
{
    LOG_DEBUG("called\n");
    mSrvSender.activateEffect(mActiveAlarm);
}

void Model::stopAlarm()
{
    LOG_DEBUG("called\n");
    mSrvSender.stopAll();
    mActiveAlarm = {};  // clear active alarm
}

void Model::snoozeAlarm()
{
    LOG_DEBUG("called\n");
    mSrvSender.snoozeAll();
    mActiveAlarm = {};  // clear active alarm
}

std::vector<AppType::Alarm>& Model::getAlarmList()
{
    return mAlarmList;
}

void Model::alarmSetEditId(size_t id)
{
    if (id > mAlarmList.size()) { // id == mAlarmList.size() means new alarm
        return;
    }

    mEditAlarmId = id;
}

size_t Model::alarmGetEditId()
{
    return mEditAlarmId;
}

void Model::saveAlarm(size_t id, AppType::Alarm alarm)
{
    LOG_DEBUG("called\n");
    if (id < mAlarmList.size()) {
        mAlarmList[id] = alarm;
        mSrvSender.updList(mAlarmList);
        modelListener->onAlarmListUpdated(mAlarmList);
    } else if (id == mAlarmList.size()) {
        // Check if this alarm already exists, if so just overwrite it (avoid duplicates)
        // This can happen if user creates a new alarm, for the same time as an existing one, but with different effect
        // Set ID same as existing one
        for(size_t i = 0; i < mAlarmList.size(); i++) {
            if (mAlarmList[i] == alarm) {
                mEditAlarmId = i;
                mAlarmList[i] = alarm;
                mSrvSender.updList(mAlarmList);
                modelListener->onAlarmListUpdated(mAlarmList);
                return;
            }
        }

        // New alarm
        mAlarmList.push_back(alarm);
        mSrvSender.updList(mAlarmList);
    }
}

void Model::deleteAlarm(size_t id)
{
    if (id >= mAlarmList.size()) {
        return;
    }
    mAlarmList.erase(mAlarmList.begin() + id);
    mSrvSender.updList(mAlarmList);
    modelListener->onAlarmListUpdated(mAlarmList);
}



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
    // Default values
    auto *msg = mKernel.comm.allocateMessage<SDK::Message::RequestSetCapabilities>();
    if (msg) {
        msg->enMusicControl = true;
        msg->enUsbChargingScreen = true;
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

    // Redraw screen
    mInvalidate = true;
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
        case CustomMessage::ALARM_LIST:  {
            LOG_DEBUG("ALARM_LIST\n");
            auto *cmsg = static_cast<CustomMessage::AlarmList*>(msg);
            mAlarmList = cmsg->list;
            modelListener->onAlarmListUpdated(mAlarmList);
        } break;

        case CustomMessage::ACTIVATED_ALARM:  {
            LOG_DEBUG("ACTIVATED_ALARM\n");
            auto *cmsg = static_cast<CustomMessage::ActivatedAlarm*>(msg);
            mActiveAlarm = cmsg->alarm;
            modelListener->onAlarmActivated(mActiveAlarm);
        } break;

        default:
            break;
    }

    return true;
}

