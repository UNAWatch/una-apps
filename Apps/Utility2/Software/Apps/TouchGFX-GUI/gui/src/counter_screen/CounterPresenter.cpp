#include <gui/counter_screen/CounterView.hpp>
#include <gui/counter_screen/CounterPresenter.hpp>

CounterPresenter::CounterPresenter(CounterView& v)
    : view(v)
{

}

void CounterPresenter::activate()
{

}

void CounterPresenter::deactivate()
{

}

void CounterPresenter::updateCounter(uint32_t value)
{
    view.updateCounter(value);
}

void CounterPresenter::exit()
{
    model->exitApp();
}