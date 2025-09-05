#ifndef MENUWHEELITEMSELECTED_HPP
#define MENUWHEELITEMSELECTED_HPP

#include <gui_generated/containers/MenuWheelItemSelectedBase.hpp>

class MenuWheelItemSelected : public MenuWheelItemSelectedBase
{
public:
    MenuWheelItemSelected();
    virtual ~MenuWheelItemSelected() {}

    virtual void initialize();

    void config(TypedTextId msgId);

    void configToggle(TypedTextId msgIdLeft, TypedTextId msgIdRight);
    void setToggle(bool state);
    bool getToggle();

    MenuWheelItemSelected& operator=(const MenuWheelItemSelected& other);

protected:
    enum ActiveStyle {
        STYLE_SIMPLE = 0,   // Center
        STYLE_TOGGLE,       // Text + toggle + Text
    };

    ActiveStyle mStyle = STYLE_SIMPLE;

    TypedTextId mMsgId = TYPED_TEXT_INVALID;
    TypedTextId mMsgIdLeft = TYPED_TEXT_INVALID;
    TypedTextId mMsgIdRight = TYPED_TEXT_INVALID;

    void reset();
    void updStyle();
};

#endif // MENUWHEELITEMSELECTED_HPP
