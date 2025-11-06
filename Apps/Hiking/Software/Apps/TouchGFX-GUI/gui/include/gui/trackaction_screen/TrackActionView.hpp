#ifndef TRACKACTIONVIEW_HPP
#define TRACKACTIONVIEW_HPP

#include <gui_generated/trackaction_screen/TrackActionViewBase.hpp>
#include <gui/trackaction_screen/TrackActionPresenter.hpp>

class TrackActionView : public TrackActionViewBase
{
public:
    TrackActionView();
    virtual ~TrackActionView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setPositionId(uint16_t id);
    uint16_t getPositionId();
    void setUnitsImperial(bool isImperial);
    void setTimer(std::time_t sec);
    void setAvgPace(float spm);
    void setDistance(float m);
    void setSteps(uint32_t steps);
    void setAvgHR(float v);
    void setFloors(uint32_t floors);

protected:
    bool mUnitsImperial = false;
    uint16_t mCounter = Gui::Config::kTrackTitleInfoSwitchPeriod;
    static const uint32_t skTitleInfoMsgNum = 6;
    uint16_t mTitleInfoMsgId = 0;

    std::time_t mTimerSec = 0;
    float mAvgPace = 0.0f;  // s/m
    float mDistance = 0.0f; // m
    uint32_t mSteps = 0;
    float mAvgHr = 0.0f;
    uint32_t mFloors = 0;

    virtual void handleKeyEvent(uint8_t key) override;
    virtual void handleTickEvent() override;
    void updTitleInfo();
};

#endif // TRACKACTIONVIEW_HPP
