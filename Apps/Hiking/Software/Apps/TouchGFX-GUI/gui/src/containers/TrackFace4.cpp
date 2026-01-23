#include <gui/containers/TrackFace4.hpp>

TrackFace4::TrackFace4()
{

}

void TrackFace4::initialize()
{
    TrackFace4Base::initialize();
}

void TrackFace4::setTime(uint8_t h, uint8_t m)
{
    if (mHour != h || mMinute != m) {
        mHour = h;
        mMinute = m;
        Unicode::snprintf(valueDayTimeBuffer, VALUEDAYTIME_SIZE, "%u:%02u", h, m);
        valueDayTime.invalidate();
    }
}

void TrackFace4::setBatteryLevel(uint8_t level)
{
    battery.setBatteryLevel(level);

    Unicode::snprintf(valuePercentBuffer, VALUEPERCENT_SIZE, "%u%s", level, touchgfx::TypedText(T_TEXT_PERCENT).getText());

    valuePercent.invalidate();
}
