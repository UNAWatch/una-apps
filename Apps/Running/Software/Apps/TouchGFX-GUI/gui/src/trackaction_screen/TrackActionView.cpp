#include <gui/trackaction_screen/TrackActionView.hpp>
#include <images/BitmapDatabase.hpp>

TrackActionView::TrackActionView()
{

}

void TrackActionView::setupScreen()
{
    TrackActionViewBase::setupScreen();

    updTitleInfo();

    menu.showTitle(false);  // Hide title

    menu.getButtons().setL1(Buttons::NONE);
    menu.getButtons().setL2(Buttons::NONE);
    menu.getButtons().setR1(Buttons::AMBER);
    menu.getButtons().setR2(Buttons::NONE);

    menu.setNumberOfItems(App::Menu::Start::Action::ID_COUNT);
    menu.setBackground(BITMAP_BACKGROUND_TEAL_ID);

    MenuItemSelected *pS = nullptr;
    MenuItemNotSelected *pN = nullptr;

    pS = menu.getSelectedItem(App::Menu::Start::Action::ID_RESUME);
    pS->config(T_TEXT_RESUME);

    pN = menu.getNotSelectedItem(App::Menu::Start::Action::ID_RESUME);
    pN->config(T_TEXT_RESUME);


    pS = menu.getSelectedItem(App::Menu::Start::Action::ID_SAVE);
    pS->config(T_TEXT_SAVE);

    pN = menu.getNotSelectedItem(App::Menu::Start::Action::ID_SAVE);
    pN->config(T_TEXT_SAVE);


    pS = menu.getSelectedItem(App::Menu::Start::Action::ID_DISCARD);
    pS->config(T_TEXT_DISCARD);

    pN = menu.getNotSelectedItem(App::Menu::Start::Action::ID_DISCARD);
    pN->config(T_TEXT_DISCARD);

    menu.invalidate();
}

void TrackActionView::tearDownScreen()
{
    TrackActionViewBase::tearDownScreen();
}

void TrackActionView::setPositionId(uint16_t id)
{
    menu.selectItem(id);
}

uint16_t TrackActionView::getPositionId()
{
    return menu.getSelectedItem();
}

void TrackActionView::setUnitsImperial(bool isImperial)
{
    mUnitsImperial = isImperial;
    updTitleInfo();
}

void TrackActionView::setTimer(std::time_t sec)
{
    mTimerSec = sec;
    updTitleInfo();
}

void TrackActionView::setAvgPace(float spm)
{
    mAvgPace = spm;
    updTitleInfo();
}

void TrackActionView::setDistance(float m)
{
    mDistance = m;
    updTitleInfo();
}

void TrackActionView::setAvgHR(float v)
{
    mAvgHr = v;
    updTitleInfo();
}

void TrackActionView::setElevation(float m)
{
    mElevation = m;
    updTitleInfo();
}

void TrackActionView::setGpsFix(bool state)
{
    mGpsFix = state;
}

void TrackActionView::handleKeyEvent(uint8_t key)
{
    if (key == Gui::Config::Button::L1) {
        menu.selectPrev();
        updTitleInfo();
    }

    if (key == Gui::Config::Button::L2) {
        menu.selectNext();
        updTitleInfo();
    }

    if (key == Gui::Config::Button::R1) {
        uint16_t id = menu.getSelectedItem();

        switch (id) {
            case App::Menu::Start::Action::ID_RESUME:
                application().gotoTrackScreenNoTransition();
                break;
            case App::Menu::Start::Action::ID_SAVE:
                application().gotoTrackSavedScreenNoTransition();
                break;
            case App::Menu::Start::Action::ID_DISCARD:
                application().gotoTrackDiscardConfirmationScreenNoTransition();
                break;
            default:
                break;
        }

    }

    if (key == Gui::Config::Button::R2) {

    }
}

void TrackActionView::handleTickEvent()
{
    if (mCounter > 0) {
        mCounter--;
    }
    if (mCounter == 0) {
        mCounter = Gui::Config::kTrackTitleInfoSwitchPeriod;

        mTitleInfoMsgId++;
        if (mTitleInfoMsgId == skTitleInfoMsgNum) {
            mTitleInfoMsgId = 0;
        }
        updTitleInfo();
    }
}

void TrackActionView::updTitleInfo()
{
    if (menu.getSelectedItem() == App::Menu::Start::Action::ID_SAVE) {
        tick.setVisible(true);
        titleInfo.setShortLine(true);
    } else {
        tick.setVisible(false);
        titleInfo.setShortLine(false);
    }
    tick.invalidate();
    const uint16_t bufferSize = 10;
    touchgfx::Unicode::UnicodeChar buffer[bufferSize] {};

    if (mTitleInfoMsgId == 0) {
        titleInfo.setTitle(T_TEXT_TIMER_UC);
        Unicode::snprintf(buffer, bufferSize, "%d:%02d",
            App::Utils::sec2hmsH(mTimerSec), App::Utils::sec2hmsM(mTimerSec));
    } else if (mTitleInfoMsgId == 1) {
        titleInfo.setTitle(T_TEXT_AVG_DOT_PACE_UC);
        std::time_t secPerKm = static_cast<std::time_t>(mAvgPace * 1000.0f);
        if (mGpsFix || secPerKm != 0) {
            if (mUnitsImperial) {
                secPerKm = static_cast<std::time_t>(secPerKm / App::Utils::km2mi(1.0f));
            }
            Unicode::snprintf(buffer, bufferSize, "%d:%02d", App::Utils::sec2hmsM(secPerKm), App::Utils::sec2hmsS(secPerKm));
        } else {
            Unicode::snprintf(buffer, bufferSize, "---");
        }
    } else if (mTitleInfoMsgId == 2) {
        titleInfo.setTitle(T_TEXT_DISTANCE_UC);
        if (mGpsFix || mDistance > 0.001f) {
            if (mUnitsImperial) {
                Unicode::snprintfFloat(buffer, bufferSize, "%.02f", App::Utils::km2mi(mDistance / 1000.0f));    // mi
            } else {
                Unicode::snprintfFloat(buffer, bufferSize, "%.02f", mDistance / 1000.0f); // km
            }
        } else {
            Unicode::snprintf(buffer, bufferSize, "---");
        }
    } else if (mTitleInfoMsgId == 3) {
        titleInfo.setTitle(T_TEXT_AVG_DOT_HR);
        Unicode::snprintfFloat(buffer, bufferSize, "%.0f", mAvgHr);
    } else if (mTitleInfoMsgId == 4) {
        titleInfo.setTitle(T_TEXT_ELEVATION_UC);
        Unicode::snprintfFloat(buffer, bufferSize, "%.0f", mElevation);
    }

    titleInfo.setValue(buffer);
}
