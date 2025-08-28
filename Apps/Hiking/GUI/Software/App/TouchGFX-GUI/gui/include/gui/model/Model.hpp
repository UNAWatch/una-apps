#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <memory>

#include "Interfaces/IModel.hpp"
#include "Interfaces/IKernel.hpp"
#include "touchgfx/UIEventListener.hpp"
#include "gui/common/GuiConfig.hpp"

#include "TrackMapBuilder.hpp"

#include "Common/Libs/Header/Settings.hpp"
#include "Common/Libs/Header/ActivitySummary.hpp"



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

    // Time
    void getDate(uint16_t& year, uint8_t& month, uint8_t& day, uint8_t& wday);
    void getTime(uint8_t& hour, uint8_t& minute, uint8_t& sec);
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
    const Gui::TrackInfo& getTrackInfo() const;
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
    //void handleEvent(const S2GEvent::Counter& event) override;

    const IKernel*             mKernel;
    ModelListener*             modelListener;
    std::shared_ptr<IGUIModel> mGSModel;

    // User data
    uint32_t mIdleTimer = 0;

    uint32_t                   mCounter;
};

#endif // MODEL_HPP
