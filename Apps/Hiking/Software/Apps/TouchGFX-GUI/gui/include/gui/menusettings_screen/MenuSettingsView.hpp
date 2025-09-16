#ifndef MENUSETTINGSVIEW_HPP
#define MENUSETTINGSVIEW_HPP

#include <gui_generated/menusettings_screen/MenuSettingsViewBase.hpp>
#include <gui/menusettings_screen/MenuSettingsPresenter.hpp>

class MenuSettingsView : public MenuSettingsViewBase
{
public:
    MenuSettingsView();
    virtual ~MenuSettingsView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setGpsFix(bool state);
    void setAutoPause(bool state);
    void setPhoneNotif(bool state);
    void setPositionId(uint16_t id);
    uint16_t getPositionId();

protected:
    bool mGpsFix { };
    bool mAutoPause { };
    bool mPhoneNotif { };

    virtual void handleKeyEvent(uint8_t key) override;
};

#endif // MENUSETTINGSVIEW_HPP
