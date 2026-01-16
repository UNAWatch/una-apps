#ifndef TRACKFACE3_HPP
#define TRACKFACE3_HPP

#include <gui_generated/containers/TrackFace3Base.hpp>

class TrackFace3 : public TrackFace3Base
{
public:
    TrackFace3();
    virtual ~TrackFace3() {}

    virtual void initialize();

    void setLapPace(float spm, bool isImperial, bool gpsFix);
    void setLapDistance(float m, bool isImperial, bool gpsFix);
    void setLapTimer(std::time_t sec);

protected:
};

#endif // TRACKFACE3_HPP
