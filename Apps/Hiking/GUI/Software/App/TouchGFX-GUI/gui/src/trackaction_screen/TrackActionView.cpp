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

    menu.setNumberOfItems(Gui::Menu::Start::Action::ID_COUNT);
    menu.setBackground(BITMAP_BACKGROUND_TEAL_ID);

    MenuItemSelected *pS = nullptr;
    MenuItemNotSelected *pN = nullptr;

    pS = menu.getSelectedItem(Gui::Menu::Start::Action::ID_RESUME);
    pS->config(T_TEXT_RESUME);

    pN = menu.getNotSelectedItem(Gui::Menu::Start::Action::ID_RESUME);
    pN->config(T_TEXT_RESUME);


    pS = menu.getSelectedItem(Gui::Menu::Start::Action::ID_SAVE);
    pS->config(T_TEXT_SAVE);

    pN = menu.getNotSelectedItem(Gui::Menu::Start::Action::ID_SAVE);
    pN->config(T_TEXT_SAVE);


    pS = menu.getSelectedItem(Gui::Menu::Start::Action::ID_DISCARD);
    pS->config(T_TEXT_DISCARD);

    pN = menu.getNotSelectedItem(Gui::Menu::Start::Action::ID_DISCARD);
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

void TrackActionView::setTimer(uint32_t sec)
{
    mTimerSec = sec;
    updTitleInfo();
}

void TrackActionView::setAvgPace(uint32_t sec)
{
    mAvgPace = sec;
    updTitleInfo();
}

void TrackActionView::setDistance(float km)
{
    mDistance = km;
    updTitleInfo();
}

void TrackActionView::setSteps(uint32_t steps)
{
    mSteps = steps;
    updTitleInfo();
}

void TrackActionView::setAvgHR(float v)
{
    mAvgHr = v;
    updTitleInfo();
}

void TrackActionView::setCalories(uint32_t kcal)
{
    mCalories = kcal;
    updTitleInfo();
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
            case Gui::Menu::Start::Action::ID_RESUME:
                application().gotoTrackScreenNoTransition();
                break;
            case Gui::Menu::Start::Action::ID_SAVE:
                application().gotoTrackSavedScreenNoTransition();
                break;
            case Gui::Menu::Start::Action::ID_DISCARD:
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
    if (menu.getSelectedItem() == Gui::Menu::Start::Action::ID_SAVE) {
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
            Gui::Utils::sec2hmsH(mTimerSec), Gui::Utils::sec2hmsM(mTimerSec));
    } else if (mTitleInfoMsgId == 1) {
        titleInfo.setTitle(T_TEXT_AVG_DOT_PACE_UC);
        uint32_t sec = mAvgPace;
        if (mUnitsImperial) {
            sec = static_cast<uint32_t>(sec / Gui::Utils::km2mi(1.0f));
        }
        Unicode::snprintf(buffer, bufferSize, "%d:%02d", Gui::Utils::sec2hmsM(sec), Gui::Utils::sec2hmsS(sec));
    } else if (mTitleInfoMsgId == 2) {
        titleInfo.setTitle(T_TEXT_DISTANCE_UC);
        if (mUnitsImperial) {
            Unicode::snprintfFloat(buffer, bufferSize, "%.02f", Gui::Utils::km2mi(mDistance));
        } else {
            Unicode::snprintfFloat(buffer, bufferSize, "%.02f", mDistance);
        }
    } else if (mTitleInfoMsgId == 3) {
        titleInfo.setTitle(T_TEXT_STEPS_UC);
        Unicode::snprintf(buffer, bufferSize, "%u", mSteps);
    } else if (mTitleInfoMsgId == 4) {
        titleInfo.setTitle(T_TEXT_AVG_DOT_HR);
        Unicode::snprintfFloat(buffer, bufferSize, "%.0f", mAvgHr);
    } else if (mTitleInfoMsgId == 5) {
        titleInfo.setTitle(T_TEXT_CALORIES_UC);
        Unicode::snprintf(buffer, bufferSize, "%u", mCalories);
    }

    titleInfo.setValue(buffer);
}
