#ifndef MODEL_HPP
#define MODEL_HPP

#include "SDK/GSModel/GSModelHelper.hpp"
#include "SDK/AppSystem/AppKernel.hpp"
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
    void handleEvent(const S2GEvent::Counter& event) override;

    const SDK::Kernel*          mKernel;
    ModelListener*              modelListener;
    std::shared_ptr<GSModelGUI> mGSModel;

    // User data
    uint32_t                    mIdleTimer = 0;
    uint32_t                    mCounter;
};

#endif // MODEL_HPP
