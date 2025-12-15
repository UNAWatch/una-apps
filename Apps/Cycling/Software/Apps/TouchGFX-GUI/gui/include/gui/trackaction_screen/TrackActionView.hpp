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
    void setAvgSpeed(float mps);
    void setDistance(float m);
    void setAvgHR(float v);
    void setElevation(float m);

protected:
    bool mUnitsImperial = false;
    uint16_t mCounter = Gui::Config::kTrackTitleInfoSwitchPeriod;
    static const uint32_t skTitleInfoMsgNum = 5;
    uint16_t mTitleInfoMsgId = 0;

    std::time_t mTimerSec = 0;
    float mAvgSpeed = 0.0f;  // m/s
    float mDistance = 0.0f; // m
    float mAvgHr = 0.0f;
    float mElevation = 0.0f; // m

    virtual void handleKeyEvent(uint8_t key) override;
    virtual void handleTickEvent() override;
    void updTitleInfo();
};

#endif // TRACKACTIONVIEW_HPP
