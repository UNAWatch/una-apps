#include <gui/containers/TrackFace3.hpp>

TrackFace3::TrackFace3()
{

}

void TrackFace3::initialize()
{
    TrackFace3Base::initialize();
}

void TrackFace3::setTime(uint8_t h, uint8_t m)
{
    if (mHour != h || mMinute != m) {
        mHour = h;
        mMinute = m;
        Unicode::snprintf(valueDayTimeBuffer, VALUEDAYTIME_SIZE, "%d:%02d", h, m);
        valueDayTime.invalidate();
    }
}

void TrackFace3::setBatteryLevel(uint8_t level)
{
    battery.setBatteryLevel(level);

    Unicode::snprintf(valuePercentBuffer, VALUEPERCENT_SIZE, "%d%s", level, touchgfx::TypedText(T_TEXT_PERCENT).getText());
    valuePercent.invalidate();
    
}