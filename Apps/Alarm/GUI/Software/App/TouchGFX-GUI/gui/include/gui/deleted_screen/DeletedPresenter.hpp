#ifndef DELETEDPRESENTER_HPP
#define DELETEDPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class DeletedView;

class DeletedPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    DeletedPresenter(DeletedView& v);

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

    virtual ~DeletedPresenter() {}

    void exitScreen();

private:
    DeletedPresenter();

    DeletedView& view;
};

#endif // DELETEDPRESENTER_HPP
