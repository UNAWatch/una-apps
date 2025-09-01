#ifndef TITLEINFO_HPP
#define TITLEINFO_HPP

#include <gui_generated/containers/TitleInfoBase.hpp>

class TitleInfo : public TitleInfoBase
{
public:
    TitleInfo();
    virtual ~TitleInfo() {}

    virtual void initialize();

    void setTitle(TypedTextId msgId);
    void setValue(touchgfx::Unicode::UnicodeChar *msg);
    void setShortLine(bool shortLine);

protected:
};

#endif // TITLEINFO_HPP
