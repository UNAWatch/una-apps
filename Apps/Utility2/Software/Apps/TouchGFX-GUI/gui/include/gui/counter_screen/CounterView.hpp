#ifndef COUNTERVIEW_HPP
#define COUNTERVIEW_HPP

#include <gui_generated/counter_screen/CounterViewBase.hpp>
#include <gui/counter_screen/CounterPresenter.hpp>

class CounterView : public CounterViewBase
{
public:
    CounterView();
    virtual ~CounterView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void updateCounter(uint32_t value);

protected:
    virtual void handleKeyEvent(uint8_t key) override;
};

#endif // COUNTERVIEW_HPP
