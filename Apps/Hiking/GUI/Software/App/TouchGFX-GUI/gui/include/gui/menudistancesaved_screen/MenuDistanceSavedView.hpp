#ifndef MENUDISTANCESAVEDVIEW_HPP
#define MENUDISTANCESAVEDVIEW_HPP

#include <gui_generated/menudistancesaved_screen/MenuDistanceSavedViewBase.hpp>
#include <gui/menudistancesaved_screen/MenuDistanceSavedPresenter.hpp>

class MenuDistanceSavedView : public MenuDistanceSavedViewBase
{
public:
    MenuDistanceSavedView();
    virtual ~MenuDistanceSavedView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setDistanceUnits(float km, bool isImperial);

protected:
    uint32_t mCounter = Gui::Config::kConfirmTimeout;

    virtual void handleTickEvent() override;
};

#endif // MENUDISTANCESAVEDVIEW_HPP
