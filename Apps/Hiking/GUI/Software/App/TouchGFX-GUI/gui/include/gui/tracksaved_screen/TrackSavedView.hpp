#ifndef TRACKSAVEDVIEW_HPP
#define TRACKSAVEDVIEW_HPP

#include <gui_generated/tracksaved_screen/TrackSavedViewBase.hpp>
#include <gui/tracksaved_screen/TrackSavedPresenter.hpp>

class TrackSavedView : public TrackSavedViewBase
{
public:
    TrackSavedView();
    virtual ~TrackSavedView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
    uint16_t mCounter = Gui::Config::kTrackActionConfirmScreenTimeout;
    virtual void handleTickEvent() override;
};

#endif // TRACKSAVEDVIEW_HPP
