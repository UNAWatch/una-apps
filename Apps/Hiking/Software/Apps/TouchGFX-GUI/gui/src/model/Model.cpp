#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <gui/common/FrontendApplication.hpp>

#include "SDK/Kernel/KernelProviderGUI.hpp"

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
    , mGSModel(std::static_pointer_cast<IGUIModel>(mKernel.gctrl.getContext()))
{
    mKernel.app.registerApp(this);
    mGSModel->setGUIHandler(&mKernel, this);

    // Default values
    mKernel.appCapabilities.enableMusicControl(true);
    mKernel.appCapabilities.enablePhoneNotification(true);
    mKernel.appCapabilities.enableUsbCharging(true);

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
    //LOG_DEBUG("tick\n");

    mGSModel->process(0);

    decIdleTimer();

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
    mGSModel->setGUIHandler(nullptr, nullptr);

    mKernel.system.exit();
    // This function only sets a flag. 
    // The current TouchGFX loop will be completed, meaning that depending 
    // on where this function was called, Model::tick(), Model::handleKeyEvent(), 
    // as well as handleTickEvent() and handleKeyEvent() for the 
    // current screen will be called.
    // After that, onPause() ->onStop() -> onDestroy() will be called.
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
    mGSModel->post(G2SEvent::SettingsSave{ mSettings });
}


// GPS
bool Model::getGpsFix()
{
    return mGpsFix;
}


// Track
void Model::trackStart()
{
    mGSModel->post(G2SEvent::TrackStart{});
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
    mGSModel->post(G2SEvent::TrackStop{});
}

void Model::discardTrack()
{
    mGSModel->post(G2SEvent::TrackStop{ true });
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

// IUserApp implementation

void Model::onStart()
{
    LOG_INFO("called\n");
    mGSModel->setGUIHandler(&mKernel, this); // re-set handler after stop
}

void Model::onResume()
{
    LOG_INFO("called\n");
    invalidate();   // Redraw screen
}

void Model::onStop()
{
    LOG_INFO("called\n");
    mGSModel->setGUIHandler(nullptr, nullptr);  // clear handler after stop
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
