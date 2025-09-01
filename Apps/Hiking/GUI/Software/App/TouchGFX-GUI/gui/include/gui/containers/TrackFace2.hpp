#ifndef TRACKFACE2_HPP
#define TRACKFACE2_HPP

#include <gui_generated/containers/TrackFace2Base.hpp>

class TrackFace2 : public TrackFace2Base
{
public:
    TrackFace2();
    virtual ~TrackFace2() {}

    virtual void initialize();

    void setHR(float hr);
    void setAvgPace(uint32_t sec, bool isImperial);
    void setElevation(float elevation, bool isImperial);

protected:
};

#endif // TRACKFACE2_HPP
