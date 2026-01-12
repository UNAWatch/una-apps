#include <gui/containers/HrBar.hpp>

HrBar::HrBar()
{

}

void HrBar::initialize()
{
    HrBarBase::initialize();
}

void HrBar::setHR(float hr, const std::array<uint8_t, 4>& th)
{
    hr1.setVisible(false);
    hr2.setVisible(false);
    hr3.setVisible(false);
    hr4.setVisible(false);
    hr5.setVisible(false);

    if (hr > th[3]) {
        hr5.setVisible(true);
    } else if (hr > th[2]) {
        hr4.setVisible(true);
    } else if (hr > th[1]) {
        hr3.setVisible(true);
    } else if (hr > th[0]) {
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