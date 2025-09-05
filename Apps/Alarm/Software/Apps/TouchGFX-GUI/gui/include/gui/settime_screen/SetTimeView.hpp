#ifndef SETTIMEVIEW_HPP
#define SETTIMEVIEW_HPP

#include <gui_generated/settime_screen/SetTimeViewBase.hpp>
#include <gui/settime_screen/SetTimePresenter.hpp>

class SetTimeView : public SetTimeViewBase
{
public:
    SetTimeView();
    virtual ~SetTimeView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void set(uint8_t h, uint8_t m, AppType::Alarm::Repeat repeat, AppType::Alarm::Effect effect);
protected:
    enum Position {
        HOURS = 0,
        MINUTES,
        REPEAT,
        EFFECT
    };
    Position mPosition{};

    void setPosition(Position id);
    void setActiveName(TypedTextId msgId);

    void handleKeyEvent(uint8_t key);
};

#endif // SETTIMEVIEW_HPP
