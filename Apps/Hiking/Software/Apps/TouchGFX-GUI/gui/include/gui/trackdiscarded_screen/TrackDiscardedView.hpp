#ifndef TRACKDISCARDEDVIEW_HPP
#define TRACKDISCARDEDVIEW_HPP

#include <gui_generated/trackdiscarded_screen/TrackDiscardedViewBase.hpp>
#include <gui/trackdiscarded_screen/TrackDiscardedPresenter.hpp>

class TrackDiscardedView : public TrackDiscardedViewBase
{
public:
    TrackDiscardedView();
    virtual ~TrackDiscardedView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
    uint16_t mCounter = Gui::Config::kTrackActionConfirmScreenTimeout;
    virtual void handleTickEvent() override;
};

#endif // TRACKDISCARDEDVIEW_HPP
