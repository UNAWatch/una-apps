#include <gui/containers/HrBar.hpp>

HrBar::HrBar()
{

}

void HrBar::initialize()
{
    HrBarBase::initialize();
}

void HrBar::setHR(float hr)
{
    hr1.setVisible(false);
    hr2.setVisible(false);
    hr3.setVisible(false);
    hr4.setVisible(false);
    hr5.setVisible(false);

    if (hr > Gui::Config::HeartRate::kThreshold4) {
        hr5.setVisible(true);
    } else if (hr > Gui::Config::HeartRate::kThreshold3) {
        hr4.setVisible(true);
    } else if (hr > Gui::Config::HeartRate::kThreshold2) {
        hr3.setVisible(true);
    } else if (hr > Gui::Config::HeartRate::kThreshold1) {
        hr2.setVisible(true);
    } else { 
        hr1.setVisible(true);
    }

    hr1.invalidate();
    hr2.invalidate();
    hr3.invalidate();
    hr4.invalidate();
    hr5.invalidate();
}