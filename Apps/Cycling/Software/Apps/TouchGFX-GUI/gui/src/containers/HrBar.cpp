#include <gui/containers/HrBar.hpp>

HrBar::HrBar()
    : mBars{&hr1, &hr2, &hr3, &hr4, &hr5}
{
}

void HrBar::initialize()
{
    HrBarBase::initialize();
}

void HrBar::setHR(float hr, const uint8_t* th, uint8_t count)
{
    if (th == nullptr || count == 0) {
        return;
    }

    if (count > mBars.size()) {
        count = static_cast<uint8_t>(mBars.size());
    }

    for (auto* bar : mBars) {
        bar->setVisible(false);
    }

    int activeBar = mBars.size() - 1;
    for (int i = count - 1; i >= 0; --i) {
        if (hr > th[i]) {
            break;
        }
        --activeBar;
    }

    if (activeBar >= 0) {
        mBars[activeBar]->setVisible(true);
    }

    for (auto* bar : mBars) {
        bar->invalidate();
    }
}
