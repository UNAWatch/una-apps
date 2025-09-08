#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include "SDK/GSModel/GSModelHelper.hpp"

#include "SDK/Interfaces/IKernel.hpp"
#include "SDK/GSModel/GSModelHelper.hpp"
#include "SDK/Interfaces/ISensorDriver.hpp"
#include "SDK/Interfaces/ISensorDataListener.hpp"

class Service : public IServiceModelHandler,
                public SDK::Interface::IUserApp::Callback,
                public SDK::Interface::ISensorDataListener
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

    void onNewSensorData(const SDK::Interface::ISensorDriver*             sensor,
                         const std::vector<SDK::Interface::ISensorData*>& data,
                         bool                                             first) override;

    const IKernel&                  mKernel;
    std::shared_ptr<GSModelService> mGSModel;
    bool                            mTerminate;
    uint32_t                        mCounter;
    bool                            mGUIStarted;
    SDK::Interface::ISensorDriver*  mDS;
    SDK::Interface::ISensorDriver*  mBMETemp;
    SDK::Interface::ISensorDriver*  mBMEPressure;
    SDK::Interface::ISensorDriver*  mAltimeter;
};

#endif
