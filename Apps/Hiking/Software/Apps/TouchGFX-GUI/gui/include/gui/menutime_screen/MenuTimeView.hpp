#ifndef MENUTIMEVIEW_HPP
#define MENUTIMEVIEW_HPP

#include <gui_generated/menutime_screen/MenuTimeViewBase.hpp>
#include <gui/menutime_screen/MenuTimePresenter.hpp>

class MenuTimeView : public MenuTimeViewBase
{
public:
    MenuTimeView();
    virtual ~MenuTimeView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setTime(uint32_t minutes);

protected:
    virtual void handleKeyEvent(uint8_t key) override;
};

#endif // MENUTIMEVIEW_HPP
