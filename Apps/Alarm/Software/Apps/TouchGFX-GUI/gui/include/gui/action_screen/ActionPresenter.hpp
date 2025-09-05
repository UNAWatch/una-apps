#ifndef ACTIONPRESENTER_HPP
#define ACTIONPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ActionView;

class ActionPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ActionPresenter(ActionView& v);

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

    virtual ~ActionPresenter() {}

    virtual void onIdleTimeout() override { model->switchToNextPriorityScreen(); }
    virtual void onAlarmListUpdated(const std::vector<AppType::Alarm>& list) override;

    void save(size_t id, bool state);

private:
    ActionPresenter();

    ActionView& view;
};

#endif // ACTIONPRESENTER_HPP
