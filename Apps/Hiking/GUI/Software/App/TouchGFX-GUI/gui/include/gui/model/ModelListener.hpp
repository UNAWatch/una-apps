#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>
#include <gui/common/FrontendApplication.hpp>

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }

    // Default actions
    virtual void onIdleTimeout()
    {
        if (model->trackIsActive()) {
            model->application().gotoTrackScreenNoTransition();
        } else {
            model->exitApp();
        }
    }


    virtual void onGpsFix(bool acquired) {}
    virtual void onBatteryLevel(uint8_t level) {}
    virtual void onDate(uint16_t year, uint8_t month, uint8_t day, uint8_t wday) {}
    virtual void onTime(uint8_t hour, uint8_t minute, uint8_t sec) {}
    virtual void onTrackInfo(const Gui::TrackInfo& info) {}
    virtual void onLapChanged() {}



protected:
    Model* model;

    
};

#endif // MODELLISTENER_HPP
