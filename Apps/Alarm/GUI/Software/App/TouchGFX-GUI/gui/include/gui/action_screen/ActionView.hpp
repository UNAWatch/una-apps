#ifndef ACTIONVIEW_HPP
#define ACTIONVIEW_HPP

#include <gui_generated/action_screen/ActionViewBase.hpp>
#include <gui/action_screen/ActionPresenter.hpp>

class ActionView : public ActionViewBase
{
public:
    ActionView();
    virtual ~ActionView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setAlarmState(size_t id, bool state);

protected:

    size_t mId = 0;
    virtual void handleKeyEvent(uint8_t key) override;
};

#endif // ACTIONVIEW_HPP
