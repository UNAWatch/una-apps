#include <gui/containers/MenuWheelItemNotSelected.hpp>

MenuWheelItemNotSelected::MenuWheelItemNotSelected()
{

}

void MenuWheelItemNotSelected::initialize()
{
    MenuWheelItemNotSelectedBase::initialize();
    
    reset();
}

void MenuWheelItemNotSelected::config(TypedTextId msgId)
{
    reset();

    mMsgId = msgId;
    mStyle = STYLE_SIMPLE;
}

void MenuWheelItemNotSelected::reset()
{
    mStyle = STYLE_SIMPLE;

    mMsgId = TYPED_TEXT_INVALID;
    textBuffer[0] = 0;
}

void MenuWheelItemNotSelected::updStyle()
{
    if (mMsgId != TYPED_TEXT_INVALID) {
        Unicode::snprintf(textBuffer, TEXT_SIZE, "%s", touchgfx::TypedText(mMsgId).getText());
    }

    switch (mStyle) {
    case STYLE_SIMPLE:
        text.setVisible(true);
        break;
    default:
        break;
    }

    text.invalidate();
}

MenuWheelItemNotSelected& MenuWheelItemNotSelected::operator=(const MenuWheelItemNotSelected& other)
{
    if (this != &other) {
        mStyle = other.mStyle;

        mMsgId = other.mMsgId;
        Unicode::strncpy(textBuffer, other.textBuffer, TEXT_SIZE);

        updStyle();
    }
    return *this;
}