#include <gui/containers/TimeWheelItemHoursNotSelected.hpp>

TimeWheelItemHoursNotSelected::TimeWheelItemHoursNotSelected()
{

}

void TimeWheelItemHoursNotSelected::initialize()
{
    TimeWheelItemHoursNotSelectedBase::initialize();
}

void TimeWheelItemHoursNotSelected::setValue(int16_t v)
{
    Unicode::snprintf(textBuffer, TEXT_SIZE, "%02d", v);
}