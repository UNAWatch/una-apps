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
    void setAvgPace(int32_t sec);
    void setDistance(float km);
    void setSteps(int32_t steps);
    void setAvgHR(float v);
    void setFloors(int32_t floors);

protected:
    bool mUnitsImperial {};
    uint16_t mCounter = Gui::Config::kTrackTitleInfoSwitchPeriod;
    static const uint32_t skTitleInfoMsgNum = 6;
    uint16_t mTitleInfoMsgId {};

    std::time_t mTimerSec {};
    int32_t mAvgPace {};
    float mDistance {};
    int32_t mSteps {};
    float mAvgHr {};
    int32_t mFloors {};

    virtual void handleKeyEvent(uint8_t key) override;
    virtual void handleTickEvent() override;
    void updTitleInfo();
};

#endif // TRACKACTIONVIEW_HPP
