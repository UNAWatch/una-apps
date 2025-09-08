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

void CounterView::handleKeyEvent(uint8_t key)
{
    // Hardwaare buttons
    if (Gui::Config::Button::L1 == key) {
    }

    if (Gui::Config::Button::L2 == key) {
    }

    if (Gui::Config::Button::R1 == key) {
    }

    if (Gui::Config::Button::R2 == key) {
        presenter->exit();
    }
}