#include <gui/model/Model.hpp>

#define LOG_MODULE_PRX      "Model::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include <memory>

#include <gui/model/ModelListener.hpp>
#include <gui/common/FrontendApplication.hpp>

#include "SDK/KernelManager.hpp"


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

    LOG_INFO("GUI is initialized\n");

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
//    LOG_INFO_WP("tick\n");


    mGSModel->checkS2GEvents();

    if (mInvalidate) {
        mInvalidate = false;
        application().invalidate();
    }

}

void Model::handleKeyEvent(uint8_t key)
{
    LOG_INFO("key = %c\n", static_cast<char>(key));

    // Hardwaare buttons
    if (Gui::Config::Button::L1 == key) {
        resetIdleTimer();
    }

    if (Gui::Config::Button::L2 == key) {
        resetIdleTimer();
    }

    if (Gui::Config::Button::R1 == key) {
        resetIdleTimer();
    }

    if (Gui::Config::Button::R2 == key) {
        resetIdleTimer();
    }
}

void Model::resetIdleTimer()
{
    mIdleTimer = Gui::Config::kScreenTimeoutSteps;
}

void Model::exitApp()
{
    LOG_INFO("exit from GUI\n");

    mGSModel->sendToService(G2SEvent::GuiStop{});
    mGSModel->setGUIHandler(nullptr, nullptr);
    mKernel->app.exit();
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
    mGSModel->sendToService(G2SEvent::SettingsSave{ mSettings });
}


// GPS
bool Model::getGpsFix()
{
    return mGpsFix;
}


// Track
void Model::trackStart()
{
    mGSModel->sendToService(G2SEvent::TrackStart{});
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
    mGSModel->sendToService(G2SEvent::TrackStop{});
}

void Model::discardTrack()
{
    mGSModel->sendToService(G2SEvent::TrackStop{ true });
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

// IUserApp implementation
void Model::onCreate()
{
    LOG_INFO("called\n");
}

void Model::onStart()
{
    LOG_INFO("called\n");

    mGSModel->setGUIHandler(mKernel, this); // Start receiving events from service
    mGSModel->sendToService(G2SEvent::GuiRun {});
}

void Model::onResume()
{
    LOG_INFO("called\n");
    invalidate();   // Redraw screen
}

void Model::onFrame()
{
    //LOG_INFO("called\n");
}

void Model::onPause()
{
    LOG_INFO("called\n");
}

void Model::onStop()
{
    LOG_INFO("called\n");

    mGSModel->sendToService(G2SEvent::GuiStop {});
    mGSModel->setGUIHandler(nullptr, nullptr);  // Stop receiving events from service
}

void Model::onDestroy()
{
    LOG_INFO("called\n");
}

void Model::handleEvent(const S2GEvent::Time& event)
{

    std::tm newTime = event.localTime;

    bool dateChanged = (newTime.tm_year != mTime.tm_year ||
        newTime.tm_mon != mTime.tm_mon || newTime.tm_mday != mTime.tm_mday);

    bool timeChanged = (newTime.tm_hour != mTime.tm_hour ||
        newTime.tm_min != mTime.tm_min || newTime.tm_sec != mTime.tm_sec);

    mTime = newTime;

    if (dateChanged) {
        modelListener->onDate(mTime.tm_year + 1900, mTime.tm_mon + 1, mTime.tm_mday, mTime.tm_wday);
    }

    if (timeChanged) {
        modelListener->onTime(mTime.tm_hour, mTime.tm_min, mTime.tm_sec);
    }
}

void Model::handleEvent(const S2GEvent::SettingsUpd& event)
{
    LOG_DEBUG("S2GEvent::SettingsUpd\n");
    mSettings = event.settings;
    mUnitsImperial = event.unitsImperial;
    mHrThresholds = event.hrThresholds;
}

void Model::handleEvent(const S2GEvent::Battery& event)
{
    LOG_DEBUG("S2GEvent::Battery\n");
    if (mBatteryLevel != event.level) {
        mBatteryLevel = event.level;
        modelListener->onBatteryLevel(mBatteryLevel);
    }
}

void Model::handleEvent(const S2GEvent::GpsFix& event)
{
    LOG_DEBUG("S2GEvent::GpsFix\n");
    if (mGpsFix != event.state) {
        mGpsFix = event.state;
        modelListener->onGpsFix(mGpsFix);
    }
}

void Model::handleEvent(const S2GEvent::TrackStateUpd& event)
{
    LOG_DEBUG("S2GEvent::TrackStateUpd\n");
    if (mTrackState != event.state) {
        mTrackState = event.state;
        modelListener->onTrackState(mTrackState);
    }
}

void Model::handleEvent(const S2GEvent::TrackDataUpd& event)
{
    //LOG_DEBUG("S2GEvent::TrackDataUpd\n");
    mTrackData = event.data;
    modelListener->onTrackData(mTrackData);
}

void Model::handleEvent(const S2GEvent::LapEnded& event)
{
    LOG_DEBUG("S2GEvent::LapEnded\n");
    modelListener->onLapChanged(event.lapNum);
}

void Model::handleEvent(const S2GEvent::Summary& event)
{
    LOG_DEBUG("S2GEvent::Summary\n");
    mpActivitySummary = event.summary;
    modelListener->onActivitySummary(*mpActivitySummary);
}
