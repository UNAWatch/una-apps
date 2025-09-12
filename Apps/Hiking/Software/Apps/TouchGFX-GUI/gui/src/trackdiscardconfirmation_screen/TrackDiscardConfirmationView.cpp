#include <gui/trackdiscardconfirmation_screen/TrackDiscardConfirmationView.hpp>

TrackDiscardConfirmationView::TrackDiscardConfirmationView()
{

}

void TrackDiscardConfirmationView::setupScreen()
{
    TrackDiscardConfirmationViewBase::setupScreen();

    title.set(T_TEXT_APP_NAME_UC);
}

void TrackDiscardConfirmationView::tearDownScreen()
{
    TrackDiscardConfirmationViewBase::tearDownScreen();
}

void TrackDiscardConfirmationView::handleKeyEvent(uint8_t key)
{
    if (key == Gui::Config::Button::L1) {

    }

    if (key == Gui::Config::Button::L2) {

    }

    if (key == Gui::Config::Button::R1) {
        application().gotoTrackDiscardedScreenNoTransition();
    }

    if (key == Gui::Config::Button::R2) {
        application().gotoTrackActionScreenNoTransition();
    }
}
