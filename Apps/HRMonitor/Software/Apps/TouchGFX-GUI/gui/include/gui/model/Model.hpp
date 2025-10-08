#ifndef MODEL_HPP
#define MODEL_HPP

#include "touchgfx/UIEventListener.hpp"

#include "SDK/GSModel/IGUIModel.hpp"
#include "SDK/Kernel/Kernel.hpp"

#include "gui/common/GuiConfig.hpp"
#include "GSModelEvents/G2SEvents.hpp"
#include "GSModelEvents/S2GEvents.hpp"

#include <vector>
#include <memory>

class FrontendApplication;
class ModelListener;

class Model : public touchgfx::UIEventListener,
              public SDK::Interface::IApp::Callback,
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

    /**
     * @brief Exits the application.
     * This method notifies the kernel that the application is exiting and performs
     * any necessary cleanup before termination.
     */
    void exitApp();

protected:
    ModelListener* modelListener;           ///< Pointer to model listener

    // Fields required for for GUI <-> Service communication
    const SDK::Kernel& mKernel;             ///< Reference to kernel interface
    std::shared_ptr<IGUIModel> mGSModel;    ///< Pointer to GUI-Service model interface

    bool mInvalidate = false;               ///< Request to redraw current screen

    // IUserApp implementation
    virtual void onStart()   override;
    virtual void onResume()  override;
    virtual void onStop()    override;


    // IGUIModelHandler implementation
    void handleEvent(const S2GEvent::HeartRate& event) override;

};

#endif // MODEL_HPP
