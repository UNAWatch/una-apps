#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <gui/common/FrontendApplication.hpp>
#include "SDK/KernelManager.hpp"


#define TAG                 "App::Alarm[G]"
#define LOG_MODULE_PRX      TAG"::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "Logger.h"

#if defined(SIMULATOR)
    #include "touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp"
    #include "Windows.h"
    #include <chrono>
    #include <ctime>
#endif

Model::Model()
    : mKernel(KernelManager::GetInstance().getKernel())
    , modelListener(0)
    , mGSModel(std::static_pointer_cast<GSModelGUI>(mKernel->gctrl.getContext()))
{
    mKernel->app.registerApp(this);
    mGSModel->setGUIHandler(mKernel, this);

    // Default values
    mKernel->app.enableMusicControl(true);
    mKernel->app.enablePhoneNotification(true);
    mKernel->app.enableUsbCharging(true);

    LOG_INFO("GUI [Alarm] is initialized\n");

#if defined(SIMULATOR)
    LOG_DEBUG("Application is running through simulator! \n");

    std::string fileStoreDir = Simulator::KernelHolder::Get().getFsPath();
    LOG_DEBUG("Path to files created by app:\n   [%s]\n", fileStoreDir.c_str());

    LOG_DEBUG_WP("\n"
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
    //LOG_INFO("tick\n");

    // Process events from Service only if app is in Resume state
    if (mIsRunning) {
        mGSModel->checkS2GEvents(0);
        
        decIdleTimer();
    }
}

void Model::handleKeyEvent(uint8_t key)
{
    LOG_INFO("key = %c\n", static_cast<char>(key));

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
    LOG_INFO("exit from AlarmGUI\n");

    exit(0);
    // This function only sets a flag. 
    // The current TouchGFX loop will be completed, meaning that depending 
    // on where this function was called, Model::tick(), Model::handleKeyEvent(), 
    // as well as handleTickEvent() and handleKeyEvent() for the 
    // current screen will be called.
    // After that, onPause() ->onStop() -> onDestroy() will be called.
}

void Model::switchToNextPriorityScreen()
{
    LOG_INFO("called\n");

    if (mActiveAlarm.on) {
        application().gotoAlarmScreenNoTransition();
        return;
    }

    //
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
    LOG_INFO("called\n");
    mGSModel->sendToService(G2SEvent::AlarmActiveteEffect{ mActiveAlarm });
}

void Model::stopAlarm()
{
    LOG_INFO("called\n");
    mGSModel->sendToService(G2SEvent::AlarmStopAll {});
    mActiveAlarm = {};  // clear active alarm
}

void Model::snoozeAlarm()
{
    LOG_INFO("called\n");
    mGSModel->sendToService(G2SEvent::AlarmSnoozeAll {});
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
    LOG_INFO("called\n");
    if (id < mAlarmList.size()) {
        mAlarmList[id] = alarm;
        mGSModel->sendToService(G2SEvent::AlarmSaveList{ mAlarmList });
        modelListener->onAlarmListUpdated(mAlarmList);
    } else if (id == mAlarmList.size()) {
        // Check if this alarm already exists, if so just overwrite it (avoid duplicates)
        // This can happen if user creates a new alarm, for the same time as an existing one, but with different effect
        // Set ID same as existing one
        for(size_t i = 0; i < mAlarmList.size(); i++) {
            if (mAlarmList[i] == alarm) {
                mEditAlarmId = i;
                mAlarmList[i] = alarm;
                mGSModel->sendToService(G2SEvent::AlarmSaveList{ mAlarmList });
                modelListener->onAlarmListUpdated(mAlarmList);
                return;
            }
        }

        // New alarm
        mAlarmList.push_back(alarm);
        mGSModel->sendToService(G2SEvent::AlarmSaveList{ mAlarmList });
    }
}

void Model::deleteAlarm(size_t id)
{
    if (id >= mAlarmList.size()) {
        return;
    }
    mAlarmList.erase(mAlarmList.begin() + id);
    mGSModel->sendToService(G2SEvent::AlarmSaveList{ mAlarmList });
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

// IUserApp implementation
void Model::onCreate()
{
    LOG_INFO("called\n");
}

void Model::onStart()
{
    LOG_INFO("called\n");

    mGSModel->setGUIHandler(mKernel, this); // re-set handler after stop
    mGSModel->sendToService(G2SEvent::Run{});
}

void Model::onResume()
{
    LOG_INFO("called\n");
    mIsRunning = true;
}

void Model::onFrame()
{
    //LOG_INFO("called\n");
}

void Model::onPause()
{
    LOG_INFO("called\n");
    mIsRunning = false;
}

void Model::onStop()
{
    LOG_INFO("called\n");

    mGSModel->sendToService(G2SEvent::Stop {});
    mGSModel->setGUIHandler(nullptr, nullptr);  // clear handler after stop
}

void Model::onDestroy()
{
    LOG_INFO("called\n");
}


// Events from Service
void Model::handleEvent(const S2GEvent::AlarmList& event)
{
    mAlarmList = event.list;
    modelListener->onAlarmListUpdated(mAlarmList);
}

void Model::handleEvent(const S2GEvent::Alarm& event)
{
    LOG_INFO("Alarm\n");

    // Save active alarm
    mActiveAlarm = event.alarm;

    modelListener->onAlarmActivated(mActiveAlarm);
}

