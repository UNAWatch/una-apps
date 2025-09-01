#ifndef ALARMVIEW_HPP
#define ALARMVIEW_HPP

#include <gui_generated/alarm_screen/AlarmViewBase.hpp>
#include <gui/alarm_screen/AlarmPresenter.hpp>

class AlarmView : public AlarmViewBase
{
public:
    AlarmView();
    virtual ~AlarmView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:

    uint16_t mCounter = Gui::Config::kAlarmAutoSnoozeTimeout;

    virtual void handleTickEvent() override;
    virtual void handleKeyEvent(uint8_t key) override;
};

#endif // ALARMVIEW_HPP
