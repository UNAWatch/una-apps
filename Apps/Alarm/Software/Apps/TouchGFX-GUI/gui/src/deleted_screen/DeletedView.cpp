#include <gui/deleted_screen/DeletedView.hpp>

DeletedView::DeletedView()
{

}

void DeletedView::setupScreen()
{
    DeletedViewBase::setupScreen();

    title.set(T_TEXT_ALARM_UC);

    buttons.setL1(Buttons::NONE);
    buttons.setL2(Buttons::NONE);
    buttons.setR1(Buttons::NONE);
    buttons.setR2(Buttons::NONE);
}

void DeletedView::tearDownScreen()
{
    DeletedViewBase::tearDownScreen();
}

void DeletedView::setAlarmId(size_t id)
{
    Unicode::snprintf(valueAlarmBuffer, VALUEALARM_SIZE, "%d", id + 1);
    valueAlarm.invalidate();
}

void DeletedView::handleTickEvent()
{
    if (mCounter > 0) {
        mCounter--;
    }
    if (mCounter == 0) {
        presenter->exitScreen();
    }
}