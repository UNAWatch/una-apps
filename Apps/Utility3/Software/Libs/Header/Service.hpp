#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include "SDK/AppSystem/AppKernel.hpp"
#include "SDK/GSModel/GSModelHelper.hpp"
#include "SDK/Interfaces/ISensorDriver.hpp"
#include "SDK/SensorLayer/SensorDriverConnection.hpp"
#include "SDK/Interfaces/ISensorDataListener.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserStepCounter.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserStepDetector.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserMotionDetect.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserActivityRecognition.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserAltimeter.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserFloorCounter.hpp"
#include "SDK/SensorLayer/DataParsers/SensorDataParserGPS.hpp"
#include "SDK/SwTimer/SwTimer.hpp"

class Service : public IServiceModelHandler,
                public SDK::Interface::IUserApp::Callback,
                public SDK::Interface::ISensorDataListener
{
public:
    Service();

    ~Service() override = default;

    void testSesnors();

    void run();
    void sdlNewData(const SDK::Interface::ISensorDriver*             sensor,
                    const std::vector<SDK::Interface::ISensorData*>& data,
                    bool                                             first) override;

    void handleEvent(const G2SEvent::Run& event);
    void handleEvent(const G2SEvent::Stop& event);

private:
    void onDestroy() override;

    const SDK::Kernel&              mKernel;

    std::shared_ptr<GSModelService> mGSModel;
    bool                            mTerminate;
    uint32_t                        mCounter;
    bool                            mGUIStarted;

    SDK::Sensors::DriverConnection mSensorStepCounter;
    SDK::Sensors::DriverConnection mSensorStepDetector;
    SDK::Sensors::DriverConnection mSensorMotionDetect;
    SDK::Sensors::DriverConnection mSensorActivityRecognition;
    SDK::Sensors::DriverConnection mAltimeter;
    SDK::Sensors::DriverConnection mSensorFloorCounter;
    SDK::Sensors::DriverConnection mGPS;
};

#endif
