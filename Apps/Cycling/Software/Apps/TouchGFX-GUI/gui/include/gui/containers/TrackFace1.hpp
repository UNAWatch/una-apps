#ifndef TRACKFACE1_HPP
#define TRACKFACE1_HPP

#include <gui_generated/containers/TrackFace1Base.hpp>

class TrackFace1 : public TrackFace1Base
{
public:
    TrackFace1();
    virtual ~TrackFace1() {}

    virtual void initialize();

    void setSpeed(float mps, bool isImperial);
    void setDistance(float m, bool isImperial);
    void setTimer(std::time_t sec);

protected:
};

#endif // TRACKFACE1_HPP
