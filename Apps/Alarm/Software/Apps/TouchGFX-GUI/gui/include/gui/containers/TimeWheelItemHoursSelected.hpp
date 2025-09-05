#ifndef TIMEWHEELITEMHOURSSELECTED_HPP
#define TIMEWHEELITEMHOURSSELECTED_HPP

#include <gui_generated/containers/TimeWheelItemHoursSelectedBase.hpp>

class TimeWheelItemHoursSelected : public TimeWheelItemHoursSelectedBase
{
public:
    TimeWheelItemHoursSelected();
    virtual ~TimeWheelItemHoursSelected() {}

    virtual void initialize();
    
    void setValue(int16_t v);

protected:
};

#endif // TIMEWHEELITEMHOURSSELECTED_HPP
