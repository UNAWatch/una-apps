#include <gui/containers/MenuWheel.hpp>

MenuWheel::MenuWheel()
{

}

void MenuWheel::initialize()
{
    MenuWheelBase::initialize();

    for (int i = 0; i < skMaxNumberOfItems; i++) {
        mSelItems[i].initialize();
        mNotSelItems[i].initialize();
    }
}

void MenuWheel::invalidate()
{
    for (int i = 0; i < wheel.getNumberOfItems(); i++) {
        wheel.itemChanged(i);
    }
    wheel.invalidate();
}

void MenuWheel::setNumberOfItems(int16_t numberOfItems)
{
    if (numberOfItems <= skMaxNumberOfItems) {
        wheel.setNumberOfItems(numberOfItems);
    }
}

int16_t MenuWheel::getNumberOfItems()
{
    return wheel.getNumberOfItems();
}

void MenuWheel::selectNext()
{
    if (wheel.getNumberOfItems() <= 1) {
        return;
    }

    int16_t p = wheel.getSelectedItem() + 1;
    wheel.animateToItem(p, Gui::Config::kMenuAnimationSteps);
}

void MenuWheel::selectPrev()
{
    if (wheel.getNumberOfItems() <= 1) {
        return;
    }

    int16_t p = wheel.getSelectedItem() - 1;
    wheel.animateToItem(p, Gui::Config::kMenuAnimationSteps);
}

void MenuWheel::selectItem(int16_t itemIndex)
{
    if (wheel.getNumberOfItems() <= 1) {
        return;
    }

    wheel.animateToItem(itemIndex, 0);
}

uint16_t MenuWheel::getSelectedItem()
{
    return wheel.getSelectedItem();
}

ScrollWheelWithSelectionStyle& MenuWheel::getWheel()
{
    return wheel;
}

MenuWheelItemSelected* MenuWheel::getSelectedItem(int16_t itemIndex)
{
    if (itemIndex < skMaxNumberOfItems) {
        return &mSelItems[itemIndex];
    }

    return nullptr;
}

MenuWheelItemNotSelected* MenuWheel::getNotSelectedItem(int16_t itemIndex)
{
    if (itemIndex < skMaxNumberOfItems) {
        return &mNotSelItems[itemIndex];
    }

    return nullptr;
}


void MenuWheel::wheelUpdateItem(MenuWheelItemNotSelected& item, int16_t itemIndex)
{
    item = mNotSelItems[itemIndex];
}

void MenuWheel::wheelUpdateCenterItem(MenuWheelItemSelected& item, int16_t itemIndex)
{
    item = mSelItems[itemIndex];
}