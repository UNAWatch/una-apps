#ifndef COUNTERPRESENTER_HPP
#define COUNTERPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class CounterView;

class CounterPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    CounterPresenter(CounterView& v);

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

    virtual ~CounterPresenter() {}

    void updateCounter(uint32_t value) override;

    void exit();

private:
    CounterPresenter();

    CounterView& view;
};

#endif // COUNTERPRESENTER_HPP
