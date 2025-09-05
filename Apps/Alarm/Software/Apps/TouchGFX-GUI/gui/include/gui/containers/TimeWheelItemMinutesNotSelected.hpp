#ifndef TIMEWHEELITEMMINUTESNOTSELECTED_HPP
#define TIMEWHEELITEMMINUTESNOTSELECTED_HPP

#include <gui_generated/containers/TimeWheelItemMinutesNotSelectedBase.hpp>

class TimeWheelItemMinutesNotSelected : public TimeWheelItemMinutesNotSelectedBase
{
public:
    TimeWheelItemMinutesNotSelected();
    virtual ~TimeWheelItemMinutesNotSelected() {}

    virtual void initialize();
    
    void setValue(int16_t v);

protected:
};

#endif // TIMEWHEELITEMMINUTESNOTSELECTED_HPP
