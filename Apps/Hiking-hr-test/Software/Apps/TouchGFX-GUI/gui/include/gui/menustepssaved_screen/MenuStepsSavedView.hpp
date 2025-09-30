#ifndef MENUSTEPSSAVEDVIEW_HPP
#define MENUSTEPSSAVEDVIEW_HPP

#include <gui_generated/menustepssaved_screen/MenuStepsSavedViewBase.hpp>
#include <gui/menustepssaved_screen/MenuStepsSavedPresenter.hpp>

class MenuStepsSavedView : public MenuStepsSavedViewBase
{
public:
    MenuStepsSavedView();
    virtual ~MenuStepsSavedView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setSteps(uint32_t steps);

protected:
    uint32_t mCounter = Gui::Config::kConfirmTimeout;

    virtual void handleTickEvent() override;
};

#endif // MENUSTEPSSAVEDVIEW_HPP
