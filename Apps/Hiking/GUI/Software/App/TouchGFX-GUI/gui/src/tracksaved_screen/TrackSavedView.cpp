#include <gui/tracksaved_screen/TrackSavedView.hpp>

TrackSavedView::TrackSavedView()
{

}

void TrackSavedView::setupScreen()
{
    TrackSavedViewBase::setupScreen();

    title.set(T_TEXT_APP_NAME_UC);
}

void TrackSavedView::tearDownScreen()
{
    TrackSavedViewBase::tearDownScreen();
}

void TrackSavedView::handleTickEvent()
{
    if (mCounter > 0) {
        mCounter--;
    }
    if (mCounter == 0) {
        application().gotoTrackSummaryScreenNoTransition();
    }
}
