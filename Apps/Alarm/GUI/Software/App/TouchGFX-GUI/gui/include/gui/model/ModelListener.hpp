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

    virtual void onIdleTimeout() {}

    virtual void onAlarmListUpdated(const std::vector<AppType::Alarm>& list) {}

    virtual void onAlarmActivated(const AppType::Alarm& alarm) {
        // Switch to Alarm screen from any other screen of this app
        model->application().gotoAlarmScreenNoTransition();
    }

    // The GUI has been ran to change settings.
    virtual void onRunForSettings() {}

protected:
    Model* model;

    
};

#endif // MODELLISTENER_HPP
