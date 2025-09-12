#ifndef BATTERYBIG_HPP
#define BATTERYBIG_HPP

#include <gui_generated/containers/BatteryBigBase.hpp>

class BatteryBig : public BatteryBigBase
{
public:
    BatteryBig();
    virtual ~BatteryBig() {}

    virtual void initialize();

    void setBatteryLevel(uint8_t level);
protected:
};

#endif // BATTERYBIG_HPP
