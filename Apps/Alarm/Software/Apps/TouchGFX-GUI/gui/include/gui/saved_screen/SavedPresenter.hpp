#ifndef SAVEDPRESENTER_HPP
#define SAVEDPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SavedView;

class SavedPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SavedPresenter(SavedView& v);

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

    virtual ~SavedPresenter() {}

    void exitScreen();

private:
    SavedPresenter();

    SavedView& view;
};

#endif // SAVEDPRESENTER_HPP
