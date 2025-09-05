#include <gui/containers/TimeWheelItemMinutesSelected.hpp>

TimeWheelItemMinutesSelected::TimeWheelItemMinutesSelected()
{

}

void TimeWheelItemMinutesSelected::initialize()
{
    TimeWheelItemMinutesSelectedBase::initialize();
}

void TimeWheelItemMinutesSelected::setValue(int16_t v)
{
    Unicode::snprintf(textBuffer, TEXT_SIZE, "%02d", v);
}