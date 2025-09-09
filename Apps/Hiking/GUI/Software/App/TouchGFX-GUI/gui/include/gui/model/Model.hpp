#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <memory>


#include "SDK/GSModel/GSModelHelper.hpp"

#include "touchgfx/UIEventListener.hpp"
#include "gui/common/GuiConfig.hpp"

#include "Settings.hpp"
#include "ActivitySummary.hpp"
#include "TrackInfo.hpp"



class FrontendApplication;
class ModelListener;

class Model : public touchgfx::UIEventListener,
              public Interface::IUserApp::Callback,
              public IGUIModelHandler
{
public:
    Model();

    void bind(ModelListener *listener)
    {
        modelListener = listener;
    }

    // Controls
    FrontendApplication &application();
    void tick();
    void handleKeyEvent(uint8_t c);
    void invalidate();

    /**
     * @brief Resets the idle timer.
     * This method is called when a user interaction occurs, such as a key press.
     * It prevents the application from going idle for a specified timeout period.
     */
    void resetIdleTimer();

    /**
     * @brief Exits the application.
     * This method notifies the kernel that the application is exiting and performs
     * any necessary cleanup before termination.
     */
    void exitApp();

    // Menu position
    void setMenuPosEnterMenu(uint16_t p);
    uint16_t getMenuPosEnterMenu();
    void setMenuPosTrack(uint16_t p);
    uint16_t getMenuPosTrack();
    void setMenuPosTrackAction(uint16_t p);
    uint16_t getMenuPosTrackAction();
    void setMenuPosMenuSettings(uint16_t p);
    uint16_t getMenuPosMenuSettings();
    void setMenuPosMenuAlerts(uint16_t p);
    uint16_t getMenuPosMenuAlerts();

    // Date/Time
    void getDate(uint8_t& m, uint8_t& d, uint8_t& wd);
    void getTime(uint8_t& h, uint8_t& m, uint8_t& s);
    std::tm getDateTime();

    // Power
    uint8_t getBatteryLevel();

    // Settings
    bool isUnitsImperial();
    const Settings& getSettings() const;
    void setSettings(const Settings& sett);

    // GPS
    bool getGpsFix();

    // Track
    void trackStart();
    bool trackIsActive();
    bool trackIsPaused();
    const Track::Data& getTrackData() const;
    void saveTrack();
    void discardTrack();
    bool trackIsSummaryAvailable();
    const ActivitySummary& trackSummary();

protected:

    /**
     * @brief Decrease the idle timer.
     * This method is called periodically to decrement the idle timer.
     * If the timer reaches zero, it triggers an idle timeout event.
     */
    void decIdleTimer();

    // IUserApp implementation
    virtual void onCreate()  override;
    virtual void onStart()   override;
    virtual void onResume()  override;
    virtual void onFrame()   override;
    virtual void onPause()   override;
    virtual void onStop()    override;
    virtual void onDestroy() override;

    // IGUIModelHandler implementation
    virtual void handleEvent(const S2GEvent::Time& event) override;
    virtual void handleEvent(const S2GEvent::SettingsUpd& event) override;
    virtual void handleEvent(const S2GEvent::Battery& event) override;
    virtual void handleEvent(const S2GEvent::GpsFix& event) override;
    virtual void handleEvent(const S2GEvent::TrackStateUpd& event) override;
    virtual void handleEvent(const S2GEvent::TrackDataUpd& event) override;
    virtual void handleEvent(const S2GEvent::LapEnded& event) override;
    virtual void handleEvent(const S2GEvent::Summary& event) override;

    const IKernel*             mKernel;
    ModelListener*             modelListener;
    std::shared_ptr<GSModelGUI> mGSModel;

    // User data
    uint32_t mIdleTimer = 0;
    bool mInvalidate = false;

    // Menu positions
    struct {
        uint16_t enter;
        uint16_t track;
        uint16_t action;
        uint16_t settings;
        uint16_t alerts;
    } mMenuPosition {};

    std::tm mTime {};

    // Kernel settings
    bool mUnitsImperial = false;
    std::array<uint8_t, 4> mHrThresholds { 150, 170, 190, 210 };

    // Application settings
    Settings mSettings {};

    // Kernel state
    bool mGpsFix = false;
    uint8_t mBatteryLevel = 0;

    // Track
    Track::State mTrackState {};
    std::shared_ptr<const ActivitySummary> mpActivitySummary = nullptr;
    Track::Data mTrackData {};

};

#endif // MODEL_HPP
