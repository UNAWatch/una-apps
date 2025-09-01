#include <gui/containers/TimeWheelItemMinutesNotSelected.hpp>

TimeWheelItemMinutesNotSelected::TimeWheelItemMinutesNotSelected()
{

}

void TimeWheelItemMinutesNotSelected::initialize()
{
    TimeWheelItemMinutesNotSelectedBase::initialize();
}

void TimeWheelItemMinutesNotSelected::setValue(int16_t v)
{
    Unicode::snprintf(textBuffer, TEXT_SIZE, "%02d", v);
}