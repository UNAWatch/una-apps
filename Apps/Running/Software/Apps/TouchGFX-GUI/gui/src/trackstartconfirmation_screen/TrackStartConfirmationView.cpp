#include <gui/trackstartconfirmation_screen/TrackStartConfirmationView.hpp>

TrackStartConfirmationView::TrackStartConfirmationView()
{

}

void TrackStartConfirmationView::setupScreen()
{
    TrackStartConfirmationViewBase::setupScreen();

    title.set(T_TEXT_APP_NAME_UC);
}

void TrackStartConfirmationView::tearDownScreen()
{
    TrackStartConfirmationViewBase::tearDownScreen();
}

void TrackStartConfirmationView::handleKeyEvent(uint8_t key)
{
    if (key == Gui::Config::Button::L1) {

    }

    if (key == Gui::Config::Button::L2) {

    }

    if (key == Gui::Config::Button::R1) {
        presenter->startTrack();
        application().gotoTrackScreenNoTransition();
    }

    if (key == Gui::Config::Button::R2) {
        application().gotoEnterMenuScreenNoTransition();
    }
}