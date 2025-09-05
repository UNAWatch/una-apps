#ifndef MODEL_HPP
#define MODEL_HPP

#include "Interfaces/IModel.hpp"
#include "touchgfx/UIEventListener.hpp"
#include "gui/common/GuiConfig.hpp"

#include <vector>
#include <memory>

class FrontendApplication;
class ModelListener;

class Model : public touchgfx::UIEventListener,
              public SDK::Interface::IUserApp::Callback,
              public IGUIModelHandler
{
public:
    Model();

    void bind(ModelListener *listener)
    {
        modelListener = listener;
    }

    /**
     * @brief Get the Frontend Application instance.
     * This method returns a reference to the FrontendApplication instance,
     * allowing access to application-level functionality and navigation 
     * from screen presenter.
     *
     * @return Reference to the FrontendApplication instance.
     */
    FrontendApplication &application();

    /**
     * @brief Model tick function.
     * This method is called periodically to update the model state.
     * It handles tasks such as decrementing the idle timer and processing
     * any other periodic updates required by the application.
     */
    void tick();

    /**
     * @brief Handles key events.
     * This method is called when a key event occurs. It processes the key event
     * and performs the appropriate action based on the key pressed.
     *
     * @param c The key code of the pressed key.
     */
    void handleKeyEvent(uint8_t c);

    /**
     * @brief Resets the idle timer.
     * This method is called when a user interaction occurs, such as a key press.
     * It prevents the application from going idle for a specified timeout period.
     */
    void resetIdleTimer();

    /**
     * @brief Exits the application.
     * This method notifies the kernel that the application is exiting.
     */
    void exitApp();

    /**
     * @brief Switches to the next priority screen.
     * This method checks for any active alarms or user interactions and switches
     * to the appropriate screen based on priority.
     */
    void switchToNextPriorityScreen();

    const AppType::Alarm& getActiveAlarm() const;
    void playAlarm();
    void stopAlarm();
    void snoozeAlarm();
    std::vector<AppType::Alarm>& getAlarmList();
    void alarmSetEditId(size_t id);
    size_t alarmGetEditId();
    void saveAlarm(size_t id, AppType::Alarm alarm);
    void deleteAlarm(size_t id);


protected:
    // Fields required for for GUI <-> Service communication
    const IKernel* mKernel;                 ///< Pointer to kernel interface
    ModelListener* modelListener;           ///< Pointer to model listener
    std::shared_ptr<IGUIModel> mGSModel;    ///< Pointer to GUI-Service model interface

    // IUserApp implementation required methods for app lifecycle
    virtual void onCreate()  override;
    virtual void onStart()   override;
    virtual void onResume()  override;
    virtual void onFrame()   override;
    virtual void onPause()   override;
    virtual void onStop()    override;
    virtual void onDestroy() override;


    // User application section

    /**
     * @brief Decrease the idle timer.
     * This method is called periodically to decrement the idle timer.
     * If the timer reaches zero, it triggers an idle timeout event.
     */
    void decIdleTimer();

    /**
     * @brief Checks if any key is pressed.
     * This method checks if the provided key corresponds to any of the defined keys
     * in the Gui::Config::Button enumeration.
     *
     * @param key The key to check.
     * @return true if the key is one of the defined keys, false otherwise.
     */
    bool isAnyKeyPressed(uint8_t key) const;

    // IGUIModelHandler implementation
    virtual void handleEvent(const S2GEvent::AlarmList& event) override;
    virtual void handleEvent(const S2GEvent::Alarm& event) override;


    /// Is app running (between onResume and onPause)
    bool mIsRunning = false;

    /// Idle timer (in ticks)
    uint32_t mIdleTimer = 0;

    /// Alarm list
    std::vector<AppType::Alarm> mAlarmList;

    /// Alarm ID being edited
    size_t mEditAlarmId = 0;

    /// Active Alarm data
    AppType::Alarm mActiveAlarm {};

    /// If true, the app will not exit on alarm stop/snooze
    bool mStayInApp = false;

};

#endif // MODEL_HPP
