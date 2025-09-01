#ifndef MENUTIMESAVEDVIEW_HPP
#define MENUTIMESAVEDVIEW_HPP

#include <gui_generated/menutimesaved_screen/MenuTimeSavedViewBase.hpp>
#include <gui/menutimesaved_screen/MenuTimeSavedPresenter.hpp>

class MenuTimeSavedView : public MenuTimeSavedViewBase
{
public:
    MenuTimeSavedView();
    virtual ~MenuTimeSavedView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setTime(uint32_t minutes);

protected:
    uint32_t mCounter = Gui::Config::kConfirmTimeout;

    virtual void handleTickEvent() override;
};

#endif // MENUTIMESAVEDVIEW_HPP
