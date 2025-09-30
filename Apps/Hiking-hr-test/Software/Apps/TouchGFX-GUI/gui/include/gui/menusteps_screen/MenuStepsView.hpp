#ifndef MENUSTEPSVIEW_HPP
#define MENUSTEPSVIEW_HPP

#include <gui_generated/menusteps_screen/MenuStepsViewBase.hpp>
#include <gui/menusteps_screen/MenuStepsPresenter.hpp>

class MenuStepsView : public MenuStepsViewBase
{
public:
    MenuStepsView();
    virtual ~MenuStepsView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setSteps(uint32_t steps);

protected:
    void handleKeyEvent(uint8_t key) override;
};

#endif // MENUSTEPSVIEW_HPP
