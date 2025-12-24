#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include "Commands.hpp"
#include "SDK/Kernel/KernelProviderService.hpp"
#include "SDK/Interfaces/ISensorDataListener.hpp"
#include "SDK/SensorLayer/SensorConnection.hpp"

#include "ActivityWriter.hpp"

class Service : public SDK::Interface::ISensorDataListener
{
public:
    Service();

    virtual ~Service() = default;

    void run();

private:
    const SDK::Kernel&           mKernel;
    CustomMessage::ServiceSender mSender;
    bool                         mTerminate;
    bool                         mGUIStarted;
    SDK::Sensor::Connection      mHRSensor;
    float                        mHR   = 0;
    float                        mHRTL = 0;
    ActivityWriter               mActivityWriter;

    virtual void onStart()    override;
    virtual void onStop()     override;
    virtual void onStartGUI() override;
    virtual void onStopGUI()  override;

    void onSdlNewData(uint16_t                 handle,
                      const SDK::Sensor::Data* data,
                      uint16_t                 count,
                      uint16_t                 stride) override;

    static uint32_t ParseVersion(const char* str);
};

#endif
