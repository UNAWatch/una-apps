#include <gui/containers/MenuWheelItemSelected.hpp>

MenuWheelItemSelected::MenuWheelItemSelected()
{

}

void MenuWheelItemSelected::initialize()
{
    MenuWheelItemSelectedBase::initialize();

    reset();

    toggleSwitch.setBackgroundGray(true);
}

void MenuWheelItemSelected::config(TypedTextId msgId)
{
    reset();

    mMsgId = msgId;
    mStyle = STYLE_SIMPLE;
}

void MenuWheelItemSelected::configToggle(TypedTextId msgIdLeft, TypedTextId msgIdRight)
{
    reset();

    mMsgIdLeft = msgIdLeft;
    mMsgIdRight = msgIdRight;
    mStyle = STYLE_TOGGLE;
}

void MenuWheelItemSelected::setToggle(bool state)
{
    toggleSwitch.setState(state);
}

bool MenuWheelItemSelected::getToggle()
{
    return toggleSwitch.getState();
}

void MenuWheelItemSelected::reset()
{
    mStyle = STYLE_SIMPLE;

    mMsgId = TYPED_TEXT_INVALID;
    textBuffer[0] = 0;

    mMsgIdLeft = TYPED_TEXT_INVALID;
    textToggleLeftBuffer[0] = 0;

    mMsgIdRight = TYPED_TEXT_INVALID;
    textToggleRightBuffer[0] = 0;
}

void MenuWheelItemSelected::updStyle()
{
    if (mMsgId != TYPED_TEXT_INVALID) {
        Unicode::snprintf(textBuffer, TEXT_SIZE, "%s", touchgfx::TypedText(mMsgId).getText());
    }

    if (mMsgIdLeft != TYPED_TEXT_INVALID) {
        Unicode::snprintf(textToggleLeftBuffer, TEXTTOGGLELEFT_SIZE, "%s", touchgfx::TypedText(mMsgIdLeft).getText());
    }

    if (mMsgIdRight != TYPED_TEXT_INVALID) {
        Unicode::snprintf(textToggleRightBuffer, TEXTTOGGLERIGHT_SIZE, "%s", touchgfx::TypedText(mMsgIdRight).getText());
    }

    switch (mStyle) {
    case STYLE_SIMPLE:
        text.setVisible(true);

        toggleSwitch.setVisible(false);
        textToggleLeft.setVisible(false);
        textToggleRight.setVisible(false);
        break;

    case STYLE_TOGGLE:
        text.setVisible(false);

        toggleSwitch.setVisible(true);
        textToggleLeft.setVisible(true);
        textToggleRight.setVisible(true);
        break;

    default:
        break;
    }

    text.invalidate();
    toggleSwitch.invalidate();
    textToggleLeft.invalidate();
    textToggleRight.invalidate();
}

MenuWheelItemSelected& MenuWheelItemSelected::operator=(const MenuWheelItemSelected& other)
{
    if (this != &other) {
        mStyle = other.mStyle;

        mMsgId = other.mMsgId;
        Unicode::strncpy(textBuffer, other.textBuffer, TEXT_SIZE);

        mMsgIdLeft = other.mMsgIdLeft;
        Unicode::strncpy(textToggleLeftBuffer, other.textToggleLeftBuffer, TEXTTOGGLELEFT_SIZE);

        mMsgIdRight = other.mMsgIdRight;
        Unicode::strncpy(textToggleRightBuffer, other.textToggleRightBuffer, TEXTTOGGLERIGHT_SIZE);

        toggleSwitch.setState(other.toggleSwitch.getState());

        updStyle();
    }
    return *this;
}