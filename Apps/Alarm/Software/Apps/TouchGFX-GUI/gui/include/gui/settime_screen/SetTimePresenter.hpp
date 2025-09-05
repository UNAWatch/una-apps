#ifndef SETTIMEPRESENTER_HPP
#define SETTIMEPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SetTimeView;

class SetTimePresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SetTimePresenter(SetTimeView& v);

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

    virtual ~SetTimePresenter() {}

    virtual void onIdleTimeout() override { model->switchToNextPriorityScreen(); }

    void exitScreen();
    void save(uint8_t h, uint8_t m, AppType::Alarm::Repeat repeat, AppType::Alarm::Effect effect);

private:
    SetTimePresenter();

    SetTimeView& view;
};

#endif // SETTIMEPRESENTER_HPP
