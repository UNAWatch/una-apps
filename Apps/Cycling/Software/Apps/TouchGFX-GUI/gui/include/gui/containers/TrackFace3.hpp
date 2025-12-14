#ifndef TRACKFACE3_HPP
#define TRACKFACE3_HPP

#include <gui_generated/containers/TrackFace3Base.hpp>

class TrackFace3 : public TrackFace3Base
{
public:
    TrackFace3();
    virtual ~TrackFace3() {}

    virtual void initialize();

    void setTime(uint8_t h, uint8_t m);
    void setBatteryLevel(uint8_t level);
protected:
    uint8_t mHour {};
    uint8_t mMinute {};
};

#endif // TRACKFACE4_HPP
