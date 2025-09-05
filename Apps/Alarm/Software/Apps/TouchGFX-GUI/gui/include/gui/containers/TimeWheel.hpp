#ifndef TIMEWHEEL_HPP
#define TIMEWHEEL_HPP

#include <gui_generated/containers/TimeWheelBase.hpp>

class TimeWheel : public TimeWheelBase
{
public:
    TimeWheel();
    virtual ~TimeWheel() {}

    virtual void initialize();

    void setTime(uint8_t h, uint8_t m);
    void getTime(uint8_t& h, uint8_t& m);

    void setActiveHours();
    void setActiveMinutes();

    void incValue();
    void decValue();

protected:

    bool mMinutesAactive{};
    virtual void hoursWheelUpdateItem(TimeWheelItemHoursNotSelected& item, int16_t itemIndex) override;
    virtual void hoursWheelUpdateCenterItem(TimeWheelItemHoursSelected& item, int16_t itemIndex) override;
    virtual void minutesWheelUpdateItem(TimeWheelItemMinutesNotSelected& item, int16_t itemIndex) override;
    virtual void minutesWheelUpdateCenterItem(TimeWheelItemMinutesSelected& item, int16_t itemIndex) override;

    void setHours(int16_t h);
    void setMinutes(int16_t m);
};

#endif // TIMEWHEEL_HPP
