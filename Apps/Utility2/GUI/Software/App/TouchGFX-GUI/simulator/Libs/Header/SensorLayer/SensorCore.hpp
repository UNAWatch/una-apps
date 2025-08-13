#ifndef __SENSOR_CORE_HPP
#define __SENSOR_CORE_HPP

#include "SensorLayer/DS18B20.hpp"
#include "SensorLayer/BME280.hpp"
#include "SensorLayer/Altimeter.hpp"
#include "SensorLayer/SensorManager.hpp"
#include "ISensorCore.hpp"

namespace Sensor {

    class Core : public Interface::ISensorCore
    {
    public:
        Core();

        void tick() override;

    private:
        Sensor::DS18B20   mDS18B20;
        Sensor::BME280    mBME280;
        Sensor::Altimeter mAltimeter;
    };

}

#endif
