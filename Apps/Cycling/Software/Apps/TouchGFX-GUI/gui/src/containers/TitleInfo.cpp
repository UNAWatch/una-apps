#include <gui/containers/TitleInfo.hpp>

TitleInfo::TitleInfo()
{

}

void TitleInfo::initialize()
{
    TitleInfoBase::initialize();
}

void TitleInfo::setTitle(TypedTextId msgId)
{
    Unicode::snprintf(titleBuffer, TITLE_SIZE, "%s", touchgfx::TypedText(msgId).getText());
    title.invalidate();
}

void TitleInfo::setValue(touchgfx::Unicode::UnicodeChar *msg)
{
    if (msg != nullptr) {
        Unicode::snprintf(valueBuffer, VALUE_SIZE, "%s", msg);
        value.setVisible(true);
    } else { 
        value.setVisible(false);
    }
    value.invalidate();
}

void TitleInfo::setShortLine(bool shortLine)
{
    line.setVisible(!shortLine);
    line.invalidate();
    lineShort.setVisible(shortLine);
    lineShort.invalidate();
}