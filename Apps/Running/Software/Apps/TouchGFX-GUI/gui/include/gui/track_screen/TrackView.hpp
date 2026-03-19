#ifndef TRACKVIEW_HPP
#define TRACKVIEW_HPP

#include <gui_generated/track_screen/TrackViewBase.hpp>
#include <gui/track_screen/TrackPresenter.hpp>

class TrackView : public TrackViewBase
{
public:
    TrackView();
    virtual ~TrackView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setPositionId(uint16_t id);
    uint16_t getPositionId();
    void setTrackData(const Track::Data &data, bool isImperial, const std::array<uint8_t, kHrThresholdsCount>& hrth);
    void setTime(uint8_t h, uint8_t m);
    void setBatteryLevel(uint8_t level);
    void setCharging(bool state);
    void setGpsFix(bool state);

protected:
    virtual void handleKeyEvent(uint8_t key) override;

    bool mGpsFix = false;
    uint16_t mGpsFixBlinkCounter = 0;
    virtual void handleTickEvent() override;
};

#endif // TRACKVIEW_HPP
