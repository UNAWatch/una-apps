#ifndef TRACKFACE2_HPP
#define TRACKFACE2_HPP

#include <gui_generated/containers/TrackFace2Base.hpp>

class TrackFace2 : public TrackFace2Base
{
public:
    TrackFace2();
    virtual ~TrackFace2() {}

    virtual void initialize();

    void setHR(float hr, float tl, const std::array<uint8_t, 4>& th);
    void setLapPace(float spm, bool isImperial);
    void setLapDistance(float m, bool isImperial);
    void setLapTimer(std::time_t sec);

protected:
};

#endif // TRACKFACE2_HPP
