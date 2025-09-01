#ifndef TIMEWHEELITEMMINUTESSELECTED_HPP
#define TIMEWHEELITEMMINUTESSELECTED_HPP

#include <gui_generated/containers/TimeWheelItemMinutesSelectedBase.hpp>

class TimeWheelItemMinutesSelected : public TimeWheelItemMinutesSelectedBase
{
public:
    TimeWheelItemMinutesSelected();
    virtual ~TimeWheelItemMinutesSelected() {}

    virtual void initialize();

    void setValue(int16_t v);

protected:
};

#endif // TIMEWHEELITEMMINUTESSELECTED_HPP
