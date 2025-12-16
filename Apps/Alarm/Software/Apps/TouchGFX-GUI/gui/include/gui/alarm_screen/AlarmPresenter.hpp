#ifndef ALARMPRESENTER_HPP
#define ALARMPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class AlarmView;

class AlarmPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    AlarmPresenter(AlarmView& v);

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

    virtual ~AlarmPresenter() {}

    void play();
    void stop();
    void snooze();

private:
    AlarmPresenter();

    AlarmView& view;
};

#endif // ALARMPRESENTER_HPP
