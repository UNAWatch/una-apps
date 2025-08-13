#include <gui/counter_screen/CounterView.hpp>

CounterView::CounterView()
{
}

void CounterView::setupScreen()
{
    CounterViewBase::setupScreen();
}

void CounterView::tearDownScreen()
{
    CounterViewBase::tearDownScreen();
}

void CounterView::updateCounter(uint32_t value)
{
    Unicode::snprintf(counterTextBuffer, COUNTERTEXT_SIZE, "%d", value);  // %lu для uint32_t
    counterText.invalidate();
}
