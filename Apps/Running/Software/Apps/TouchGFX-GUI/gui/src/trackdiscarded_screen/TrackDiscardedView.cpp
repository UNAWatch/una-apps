#include <gui/trackdiscarded_screen/TrackDiscardedView.hpp>

TrackDiscardedView::TrackDiscardedView()
{

}

void TrackDiscardedView::setupScreen()
{
    TrackDiscardedViewBase::setupScreen();

    title.set(T_TEXT_APP_NAME_UC);
}

void TrackDiscardedView::tearDownScreen()
{
    TrackDiscardedViewBase::tearDownScreen();
}

void TrackDiscardedView::handleTickEvent()
{
    if (mCounter > 0) {
        mCounter--;
    }
    if (mCounter == 0) {
        presenter->exitApp();
    }
}
