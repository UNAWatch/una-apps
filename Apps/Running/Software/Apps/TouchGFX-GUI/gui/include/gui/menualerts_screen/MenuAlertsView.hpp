#ifndef MENUALERTSVIEW_HPP
#define MENUALERTSVIEW_HPP

#include <gui_generated/menualerts_screen/MenuAlertsViewBase.hpp>
#include <gui/menualerts_screen/MenuAlertsPresenter.hpp>

class MenuAlertsView : public MenuAlertsViewBase
{
public:
    MenuAlertsView();
    virtual ~MenuAlertsView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setUnitsImperial(bool isImperial);
    void setDistance(float km);
    void setTime(uint32_t minutes);
    void setPositionId(uint16_t id);
    uint16_t getPositionId();

protected:
    bool mUnitsImperial = false;
    float mDistanceKm = 0.0f;
    uint32_t mTime = 0;

    virtual void handleKeyEvent(uint8_t key) override;
    void update();
};

#endif // MENUALERTSVIEW_HPP
