#ifndef TRACKFACE3_HPP
#define TRACKFACE3_HPP

#include <gui_generated/containers/TrackFace3Base.hpp>

class TrackFace3 : public TrackFace3Base
{
public:
    TrackFace3();
    virtual ~TrackFace3() {}

    virtual void initialize();

    void setSpeed(float spm, bool isImperial, bool gpsFix);
    void setDistance(float m, bool isImperial, bool gpsFix);
    void setTimer(std::time_t sec);
protected:

};

#endif // TRACKFACE4_HPP
