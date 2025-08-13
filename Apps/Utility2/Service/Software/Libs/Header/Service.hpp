#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include "Interfaces/IKernel.hpp"
#include "GSModel.hpp"
#include "SensorLayer/ISensorDriver.hpp"
#include "SensorLayer/ISensorDataListener.hpp"

class Service : public IServiceModelHandler,
                public Interface::IUserApp::Callback,
                public Interface::ISensorDataListener
{
public:
    Service(const IKernel& kernel);

    virtual ~Service() = default;

    void run();
    void handleEvent(const G2SEvent::Run& event);
    void handleEvent(const G2SEvent::Stop& event);

private:
    void onCreate()  override;
    void onStart()   override;
    void onResume()  override;
    void onStop()    override;
    void onPause()   override;
    void onDestroy() override;

    void onNewSensorData(const Interface::ISensorDriver*             sensor,
                         const std::vector<Interface::ISensorData*>& data,
                         bool                                        first) override;

    const IKernel&            mKernel;
    std::shared_ptr<GSModel>  mGSModel;
    bool                      mTerminate;
    uint32_t                  mCounter;
    bool                      mGUIStarted;
    Interface::ISensorDriver* mDS;
    Interface::ISensorDriver* mBMETemp;
    Interface::ISensorDriver* mBMEPressure;
    Interface::ISensorDriver* mAltimeter;
};

#endif
