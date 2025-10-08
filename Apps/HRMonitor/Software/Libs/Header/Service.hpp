#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include "SDK/GSModel/GSModel.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"
#include "SDK/Interfaces/ISensorDataListener.hpp"
#include "SDK/SensorLayer/SensorDriverConnection.hpp"

#include "ActivityWriter.hpp"

class Service : public IServiceModelHandler,
                public SDK::Interface::IApp::Callback,
                public SDK::Interface::ISensorDataListener
{
public:
    Service();

    virtual ~Service() = default;

    void run();

private:
    const SDK::Kernel& mKernel;
    GSModel            mGSModel;

    bool               mTerminate;
    bool               mGUIStarted;

    virtual void onStart()    override;
    virtual void onStop()     override;
    virtual void onStartGUI() override;
    virtual void onStopGUI()  override;

    void onSdlNewData(const SDK::Interface::ISensorDriver*             sensor,
                      const std::vector<SDK::Interface::ISensorData*>& data,
                      bool                                             first) override;


    SDK::Sensor::DriverConnection mHRSensor;

    float mHR = 0;
    float mHRTL = 0;
    ActivityWriter mActivityWriter;

    static uint32_t ParseVersion(const char* str);
};

#endif
