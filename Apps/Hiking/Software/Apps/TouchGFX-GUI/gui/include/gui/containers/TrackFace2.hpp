#ifndef TRACKFACE2_HPP
#define TRACKFACE2_HPP

#include <gui_generated/containers/TrackFace2Base.hpp>

class TrackFace2 : public TrackFace2Base
{
public:
    TrackFace2();
    virtual ~TrackFace2() {}

    virtual void initialize();

    void setHR(float hr, float tl, const std::array<uint8_t, kHrThresholdsCount>& th);
    void setAvgPace(float spm, bool isImperial, bool gpsFix);
    void setElevation(float m, bool isImperial);

protected:
};

#endif // TRACKFACE2_HPP
