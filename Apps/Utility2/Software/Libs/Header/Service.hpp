#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include "SDK/GSModel/GSModel.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"
#include "SDK/SensorLayer/SensorDriverConnection.hpp"
#include "SDK/Interfaces/ISensorDataListener.hpp"

class Service : public IServiceModelHandler,
                public SDK::Interface::IUserApp::Callback,
                public SDK::Interface::ISensorDataListener
{
public:
    Service();

    ~Service() override = default;

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

    void onSdlNewData(const SDK::Interface::ISensorDriver*             sensor,
                      const std::vector<SDK::Interface::ISensorData*>& data,
                      bool                                             first) override;

    const SDK::Kernel&              mKernel;
    GSModel                         mGSModel;
    bool                            mTerminate;
    uint32_t                        mCounter;
    bool                            mGUIStarted;
    SDK::Sensors::DriverConnection  mBMETemp;
    SDK::Sensors::DriverConnection  mBMEPressure;
    SDK::Sensors::DriverConnection  mAltimeter;
};

#endif
