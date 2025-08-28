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
    void setTimer(uint32_t sec);
    void setAvgPace(uint32_t sec);
    void setDistance(float km);
    void setSteps(uint32_t steps);
    void setAvgHR(float v);
    void setCalories(uint32_t kcal);

protected:
    bool mUnitsImperial {};
    uint16_t mCounter = Gui::Config::kTrackTitleInfoSwitchPeriod;
    static const uint32_t skTitleInfoMsgNum = 6;
    uint16_t mTitleInfoMsgId {};

    uint32_t mTimerSec {};
    uint32_t mAvgPace {};
    float mDistance {};
    uint32_t mSteps {};
    float mAvgHr {};
    uint32_t mCalories {};

    virtual void handleKeyEvent(uint8_t key) override;
    virtual void handleTickEvent() override;
    void updTitleInfo();
};

#endif // TRACKACTIONVIEW_HPP
