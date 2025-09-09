#ifndef TRACKLAPVIEW_HPP
#define TRACKLAPVIEW_HPP

#include <gui_generated/tracklap_screen/TrackLapViewBase.hpp>
#include <gui/tracklap_screen/TrackLapPresenter.hpp>

class TrackLapView : public TrackLapViewBase
{
public:
    TrackLapView();
    virtual ~TrackLapView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setUnitsImperial(bool isImperial);
    void setLapNum(uint8_t n);
    void setSteps(int32_t steps);
    void setDistance(float km);
    void setTimer(std::time_t sec);

protected:
    bool mUnitsImperial {};
    uint16_t mCounter = Gui::Config::kTrackLapScreenTimeout;

    virtual void handleTickEvent() override;
    virtual void handleKeyEvent(uint8_t key) override;
};

#endif // TRACKLAPVIEW_HPP
