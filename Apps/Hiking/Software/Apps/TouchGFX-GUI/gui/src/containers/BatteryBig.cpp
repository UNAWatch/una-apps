#include <gui/containers/BatteryBig.hpp>

BatteryBig::BatteryBig()
{

}

void BatteryBig::initialize()
{
    BatteryBigBase::initialize();
}

void BatteryBig::setBatteryLevel(uint8_t level)
{
    gray1.setVisible(false);
    gray2.setVisible(false);
    gray3.setVisible(false);
    red1.setVisible(false);
    green1.setVisible(false);
    green2.setVisible(false);
    green3.setVisible(false);

    if (level >= Gui::Config::Battery::kThresholdHi) {
        green1.setVisible(true);
        green2.setVisible(true);
        green3.setVisible(true);
    } else if (level >= Gui::Config::Battery::kThresholdLo) {
        green1.setVisible(true);
        green2.setVisible(true);
        gray3.setVisible(true);
    } else if (level > 0) {
        red1.setVisible(true);
        gray2.setVisible(true);
        gray3.setVisible(true);
    } else { 
        gray1.setVisible(true);
        gray2.setVisible(true);
        gray3.setVisible(true);
    }

    gray1.invalidate();
    gray2.invalidate();
    gray3.invalidate();
    red1.invalidate();
    green1.invalidate();
    green2.invalidate();
    green3.invalidate();
}