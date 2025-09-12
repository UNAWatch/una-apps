#ifndef MENUSTEPSPRESENTER_HPP
#define MENUSTEPSPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class MenuStepsView;

class MenuStepsPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    MenuStepsPresenter(MenuStepsView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~MenuStepsPresenter() {}

    void saveSteps(uint32_t steps);

private:
    MenuStepsPresenter();

    MenuStepsView& view;
};

#endif // MENUSTEPSPRESENTER_HPP
