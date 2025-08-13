#ifndef MODEL_HPP
#define MODEL_HPP

#include "Interfaces/IModel.hpp"
#include "Interfaces/IKernel.hpp"
#include "touchgfx/UIEventListener.hpp"

#include <vector>
#include <memory>

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

    FrontendApplication &application();

    void tick();
    void exitApp();

    virtual void handleKeyEvent(uint8_t c);

protected:
    // IApp
    virtual void onCreate()  override;
    virtual void onStart()   override;
    virtual void onResume()  override;
    virtual void onFrame()   override;
    virtual void onPause()   override;
    virtual void onStop()    override;
    virtual void onDestroy() override;

    void handleEvent(const S2GEvent::Counter& event) override;

    const IKernel*             mKernel;
    ModelListener*             modelListener;
    std::shared_ptr<IGUIModel> mGSModel;
    uint32_t                   mCounter;
};

#endif // MODEL_HPP
