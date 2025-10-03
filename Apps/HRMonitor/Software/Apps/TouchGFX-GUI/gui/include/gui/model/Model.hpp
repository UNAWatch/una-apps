#ifndef MODEL_HPP
#define MODEL_HPP

#include "SDK/GSModel/IGUIModel.hpp"
#include "SDK/Kernel/Kernel.hpp"
#include "GSModelEvents/G2SEvents.hpp"
#include "GSModelEvents/S2GEvents.hpp"
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

    /**
     * @brief Exits the application.
     * This method notifies the kernel that the application is exiting and performs
     * any necessary cleanup before termination.
     */
    void exitApp();

protected:

    // IUserApp implementation
    virtual void onStart()   override;
    virtual void onStop()    override;


    // IGUIModelHandler implementation
    void handleEvent(const S2GEvent::HeartRate& event) override;

    const SDK::Kernel&         mKernel;
    ModelListener*             modelListener;
    std::shared_ptr<IGUIModel> mGSModel;
};

#endif // MODEL_HPP
