#include <gui/alarm_screen/AlarmView.hpp>
#include "Logger.h"

AlarmView::AlarmView()
{

}

void AlarmView::setupScreen()
{
    AlarmViewBase::setupScreen();
    LOG_DEBUG("AlarmView::setupScreen()\n");
}

void AlarmView::tearDownScreen()
{
    AlarmViewBase::tearDownScreen();
}


void AlarmView::handleTickEvent()
{
    if (mCounter > 0) {
        mCounter--;
    }
    if (mCounter == 0) {
        presenter->snooze();
    }
}

void AlarmView::handleKeyEvent(uint8_t key)
{
    if (key == Gui::Config::Button::L1) {

    }

    if (key == Gui::Config::Button::L2) {

    }

    if (key == Gui::Config::Button::R1) {
        presenter->stop();
    }

    if (key == Gui::Config::Button::R2) {
        presenter->snooze();
    }
}