#ifndef MENUDISTANCEVIEW_HPP
#define MENUDISTANCEVIEW_HPP

#include <gui_generated/menudistance_screen/MenuDistanceViewBase.hpp>
#include <gui/menudistance_screen/MenuDistancePresenter.hpp>

class MenuDistanceView : public MenuDistanceViewBase
{
public:
    MenuDistanceView();
    virtual ~MenuDistanceView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setDistanceUnits(float km, bool isImperial);

protected:
    bool mUnitsImperial {};

    virtual void handleKeyEvent(uint8_t key) override;
    void update();

};

#endif // MENUDISTANCEVIEW_HPP
