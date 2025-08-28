#ifndef MENUSTEPSSAVEDPRESENTER_HPP
#define MENUSTEPSSAVEDPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class MenuStepsSavedView;

class MenuStepsSavedPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    MenuStepsSavedPresenter(MenuStepsSavedView& v);

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

    virtual ~MenuStepsSavedPresenter() {}

private:
    MenuStepsSavedPresenter();

    MenuStepsSavedView& view;
};

#endif // MENUSTEPSSAVEDPRESENTER_HPP
