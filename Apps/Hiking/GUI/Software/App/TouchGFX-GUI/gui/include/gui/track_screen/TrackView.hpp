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
    void setTrackInfo(const Gui::TrackInfo &info, bool isImperial);
    void setTime(uint8_t h, uint8_t m);
    void setBatteryLevel(uint8_t level);
    void setCharging(bool state);

protected:
    virtual void handleKeyEvent(uint8_t key) override;
};

#endif // TRACKVIEW_HPP
