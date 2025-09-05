#include <gui/containers/TimeWheelItemHoursSelected.hpp>

TimeWheelItemHoursSelected::TimeWheelItemHoursSelected()
{

}

void TimeWheelItemHoursSelected::initialize()
{
    TimeWheelItemHoursSelectedBase::initialize();
}


void TimeWheelItemHoursSelected::setValue(int16_t v)
{
    Unicode::snprintf(textBuffer, TEXT_SIZE, "%02d", v);
}