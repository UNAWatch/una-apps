#ifndef MENUWHEELITEMNOTSELECTED_HPP
#define MENUWHEELITEMNOTSELECTED_HPP

#include <gui_generated/containers/MenuWheelItemNotSelectedBase.hpp>

class MenuWheelItemNotSelected : public MenuWheelItemNotSelectedBase
{
public:
    MenuWheelItemNotSelected();
    virtual ~MenuWheelItemNotSelected() {}

    virtual void initialize();

    void config(TypedTextId msgId);

    MenuWheelItemNotSelected& operator=(const MenuWheelItemNotSelected& other);
protected:

    enum ActiveStyle {
        STYLE_SIMPLE = 0,   // Center
    };

    ActiveStyle mStyle = STYLE_SIMPLE;

    TypedTextId mMsgId = TYPED_TEXT_INVALID;

    void reset();
    void updStyle();
};

#endif // MENUWHEELITEMNOTSELECTED_HPP
