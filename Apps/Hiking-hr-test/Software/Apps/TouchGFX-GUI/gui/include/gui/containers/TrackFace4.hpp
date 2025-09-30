#ifndef TRACKFACE4_HPP
#define TRACKFACE4_HPP

#include <gui_generated/containers/TrackFace4Base.hpp>

class TrackFace4 : public TrackFace4Base
{
public:
    TrackFace4();
    virtual ~TrackFace4() {}

    virtual void initialize();

    void setTime(uint8_t h, uint8_t m);
    void setBatteryLevel(uint8_t level);
protected:
    uint8_t mHour {};
    uint8_t mMinute {};
};

#endif // TRACKFACE4_HPP
