#ifndef TRACKSUMMARY_HPP
#define TRACKSUMMARY_HPP

#include <gui_generated/containers/TrackSummaryBase.hpp>
#include <gui/containers/PolyLine.hpp>
#include <touchgfx/widgets/canvas/Line.hpp>

class TrackSummary : public TrackSummaryBase
{
public:
    TrackSummary();
    virtual ~TrackSummary() {}

    virtual void initialize();

    void setDistance(float m, bool isImperial);
    void setAvgPace(float spm, bool isImperial);
    void setTimer(uint32_t sec);
    void setMap(const SDK::TrackMapScreen &map);

protected:
    PolyLine track;
    static const int16_t kBaseTrackX = 30;
    static const int16_t kBaseTrackY = 45;
};

#endif // TRACKSUMMARY_HPP
