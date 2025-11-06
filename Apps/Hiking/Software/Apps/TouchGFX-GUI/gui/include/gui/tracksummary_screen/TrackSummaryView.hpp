#ifndef TRACKSUMMARYVIEW_HPP
#define TRACKSUMMARYVIEW_HPP

#include <gui_generated/tracksummary_screen/TrackSummaryViewBase.hpp>
#include <gui/tracksummary_screen/TrackSummaryPresenter.hpp>

class TrackSummaryView : public TrackSummaryViewBase
{
public:
    TrackSummaryView();
    virtual ~TrackSummaryView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setDistance(float m, bool isImperial);
    void setSteps(uint32_t v);
    void setAvgPace(float spm, bool isImperial);
    void setElevation(float m, bool isImperial);
    void setTimer(uint32_t sec);
    void setMaxHR(float hr);
    void setAvgHR(float hr);
    void setMap(const SDK::TrackMapScreen &map);

protected:
    const uint16_t kFacesNum = 2;
    int16_t mCurrentFace {};

    virtual void handleKeyEvent(uint8_t key) override;
    void updFace();
};

#endif // TRACKSUMMARYVIEW_HPP
