#include <gui/saved_screen/SavedView.hpp>

SavedView::SavedView()
{

}

void SavedView::setupScreen()
{
    SavedViewBase::setupScreen();

    title.set(T_TEXT_ALARM_UC);

    buttons.setL1(Buttons::NONE);
    buttons.setL2(Buttons::NONE);
    buttons.setR1(Buttons::NONE);
    buttons.setR2(Buttons::NONE);
}

void SavedView::tearDownScreen()
{
    SavedViewBase::tearDownScreen();
}

void SavedView::setAlarmId(size_t id)
{
    Unicode::snprintf(valueAlarmBuffer, VALUEALARM_SIZE, "%d", id + 1);
    valueAlarm.invalidate();
}

void SavedView::handleTickEvent()
{
    if (mCounter > 0) {
        mCounter--;
    }
    if (mCounter == 0) {
        presenter->exitScreen();
    }
}