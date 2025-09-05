#ifndef TIMEWHEELITEMHOURSNOTSELECTED_HPP
#define TIMEWHEELITEMHOURSNOTSELECTED_HPP

#include <gui_generated/containers/TimeWheelItemHoursNotSelectedBase.hpp>

class TimeWheelItemHoursNotSelected : public TimeWheelItemHoursNotSelectedBase
{
public:
    TimeWheelItemHoursNotSelected();
    virtual ~TimeWheelItemHoursNotSelected() {}

    virtual void initialize();

    void setValue(int16_t v);

protected:
};

#endif // TIMEWHEELITEMHOURSNOTSELECTED_HPP
