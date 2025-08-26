#ifndef MENUWHEEL_HPP
#define MENUWHEEL_HPP

#include <gui_generated/containers/MenuWheelBase.hpp>

class MenuWheel : public MenuWheelBase
{
public:
    MenuWheel();
    virtual ~MenuWheel() {}

    virtual void initialize();

    virtual void invalidate();

    static const uint16_t skMaxNumberOfItems = 15;

    void setNumberOfItems(int16_t numberOfItems);
    int16_t getNumberOfItems();

    void selectNext();
    void selectPrev();
    void selectItem(int16_t itemIndex);
    uint16_t getSelectedItem();

    ScrollWheelWithSelectionStyle& getWheel();
    MenuWheelItemSelected* getSelectedItem(int16_t itemIndex);
    MenuWheelItemNotSelected* getNotSelectedItem(int16_t itemIndex);

protected:
    virtual void wheelUpdateItem(MenuWheelItemNotSelected& item, int16_t itemIndex) override;
    virtual void wheelUpdateCenterItem(MenuWheelItemSelected& item, int16_t itemIndex) override;

    MenuWheelItemSelected mSelItems[skMaxNumberOfItems]{ };
    MenuWheelItemNotSelected mNotSelItems[skMaxNumberOfItems]{ };
};

#endif // MENUWHEEL_HPP
