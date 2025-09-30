#ifndef ENTERMENUPRESENTER_HPP
#define ENTERMENUPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class EnterMenuView;

class EnterMenuPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    EnterMenuPresenter(EnterMenuView& v);

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

    virtual ~EnterMenuPresenter() {}

    virtual void onGpsFix(bool acquired) override;
    virtual void onActivitySummary(const ActivitySummary& summary) override;

    void startTrack();
    void exitApp();

private:
    EnterMenuPresenter();

    EnterMenuView& view;
};

#endif // ENTERMENUPRESENTER_HPP
