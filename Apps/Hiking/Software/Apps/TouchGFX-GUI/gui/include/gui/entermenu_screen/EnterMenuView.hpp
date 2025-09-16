#ifndef ENTERMENUVIEW_HPP
#define ENTERMENUVIEW_HPP

#include <gui_generated/entermenu_screen/EnterMenuViewBase.hpp>
#include <gui/entermenu_screen/EnterMenuPresenter.hpp>

class EnterMenuView : public EnterMenuViewBase
{
public:
    EnterMenuView();
    virtual ~EnterMenuView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setGpsFix(bool state);
    void setSummaryAvailable(bool state);
    void setPositionId(uint16_t id);
    uint16_t getPositionId();

protected:
    bool mGpsFix = false;
    bool mSummaryAvailable = false;
    uint16_t mBackgroundUpdDelay = 0;

    virtual void handleKeyEvent(uint8_t key) override;
    virtual void handleTickEvent() override;
    void updBackground();
};

#endif // ENTERMENUVIEW_HPP
