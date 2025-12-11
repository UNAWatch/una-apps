#ifndef MODEL_HPP
#define MODEL_HPP

#include "touchgfx/UIEventListener.hpp"

#include "SDK/Kernel/Kernel.hpp"
#include "SDK/Interfaces/IGuiLifeCycleCallback.hpp"
#include "SDK/Interfaces/ICustomMessageHandler.hpp"

#include "gui/common/GuiConfig.hpp"
#include "Commands.hpp"

#include <vector>
#include <memory>

class FrontendApplication;
class ModelListener;

class Model : public touchgfx::UIEventListener,
              public SDK::Interface::IGuiLifeCycleCallback,
              public SDK::Interface::ICustomMessageHandler
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
    ModelListener* modelListener;           ///< Pointer to model listener
    
    // Fields required for for GUI <-> Service communication
    const SDK::Kernel&      mKernel;        ///< Reference to kernel interface
    CustomMessage::Sender   mSrvSender;

    // IGuiLifeCycleCallback implementation required methods for app lifecycle
    virtual void onStart()   override;
    virtual void onResume()  override;
    virtual void onSuspend() override;
    virtual void onStop()    override;


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

    void setCapabilities();

    // ICustomMessageHandler implementation
    virtual bool customMessageHandler(SDK::MessageBase *msg) override;


    /// Is app running (between onResume and onPause)
    bool mIsRunning = false;

    /// Request to redraw current screen
    bool mInvalidate = false;

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
