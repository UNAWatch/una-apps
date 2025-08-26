#include <gui/main_screen/MainView.hpp>
#include "Logger.h"

MainView::MainView()
{

}

void MainView::setupScreen()
{
    MainViewBase::setupScreen();
    LOG_DEBUG("MainView::setupScreen()\n");

    buttons.setL1(Buttons::NONE);
    buttons.setL2(Buttons::NONE);
    buttons.setR1(Buttons::AMBER);
    buttons.setR2(Buttons::WHITE);

    toggleSwitch.setBackgroundGray(true);

    title.set(T_TEXT_ALARM_UC);

}

void MainView::tearDownScreen()
{
    MainViewBase::tearDownScreen();
}


void MainView::updateAlarmList(const std::vector<AppType::Alarm>& list)
{
    pList = &list;  // save pointer to the list

    mAlarmId = 0;

    show();
}

void MainView::setActiveAlarm(size_t id)
{
    if (pList == nullptr) {
        return;
    }


    if (id <= pList->size()) {
        mAlarmId = id;
    }

    show();
}

void MainView::handleKeyEvent(uint8_t key)
{
    if (pList == nullptr) {
        return;
    }

    // Next alarm
    if (key == Gui::Config::Button::L1) {
        mAlarmId++;
        if (mAlarmId > pList->size()) { // If equals to pList->size() -> New Alarm
            mAlarmId = 0;
        }
        show();
    }

    // Previous alarm
    if (key == Gui::Config::Button::L2) {
        if (mAlarmId > 0) { // If equals to pList->size() -> New Alarm
            mAlarmId--;
        }
        else {
            mAlarmId = pList->size();
        }
        show();
    }

    if (key == Gui::Config::Button::R1) {
        if (mAlarmId == pList->size()) {
            // Create new one
            presenter->setAlarmIdForEdit(mAlarmId);
            application().gotoSetTimeScreenNoTransition();
        }
        else {
            // Edit existed one
            presenter->setAlarmIdForEdit(mAlarmId);
            application().gotoActionScreenNoTransition();
        }
    }

    if (key == Gui::Config::Button::R2) {
        presenter->exitApp();
    }
}

void MainView::show()
{
    if (pList == nullptr) {
        return;
    }

    buttons.setL1(pList->size() > 0 ? Buttons::WHITE : Buttons::NONE);
    buttons.setL2(pList->size() > 0 ? Buttons::WHITE : Buttons::NONE);

    if (pList->size() == 0 || mAlarmId == pList->size()) {
        // New Alarm
        textNew.setVisible(true);
        icon.setVisible(true);

        textAlarm.setVisible(false);
        valueAlarm.setVisible(false);
        toggleSwitch.setVisible(false);
        time.setVisible(false);
        textRepeat.setVisible(false);
        valueRepeat.setVisible(false);
    }
    else {
        textNew.setVisible(false);
        icon.setVisible(false);

        textAlarm.setVisible(true);
        valueAlarm.setVisible(true);
        toggleSwitch.setVisible(true);
        time.setVisible(true);
        textRepeat.setVisible(true);
        valueRepeat.setVisible(true);

        Unicode::snprintf(valueAlarmBuffer, VALUEALARM_SIZE, "%d", mAlarmId + 1);

        toggleSwitch.setState((*pList)[mAlarmId].on);

        Unicode::snprintf(timeBuffer, TIME_SIZE, "%02d:%02d",
            (*pList)[mAlarmId].timeHours, (*pList)[mAlarmId].timeMinutes);

        Unicode::snprintf(valueRepeatBuffer, VALUEREPEAT_SIZE, "%s",
            touchgfx::TypedText(Gui::Config::Alarm::RepeatValue[(*pList)[mAlarmId].repeat]).getText());

        if ((*pList)[mAlarmId].repeat == AppType::Alarm::Repeat::REPEAT_NO) {
            valueRepeat.setColor(Gui::Config::Color::TEAL);
        }
        else {
            valueRepeat.setColor(Gui::Config::Color::YELLOW_DARK);
        }

    }

    textNew.invalidate();
    icon.invalidate();
    textAlarm.invalidate();
    valueAlarm.invalidate();
    toggleSwitch.invalidate();
    time.invalidate();
    textRepeat.invalidate();
    valueRepeat.invalidate();
}