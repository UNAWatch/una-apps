#include <gui/tracksummary_screen/TrackSummaryView.hpp>

TrackSummaryView::TrackSummaryView()
{

}

void TrackSummaryView::setupScreen()
{
    TrackSummaryViewBase::setupScreen();

    buttons.setL1(Buttons::NONE);
    buttons.setL2(Buttons::NONE);
    buttons.setR1(Buttons::NONE);
    buttons.setR2(Buttons::AMBER);

    title.set(T_TEXT_APP_NAME_UC);

    sideBar.setCount(kFacesNum);
    sideBar.setActiveId(0);

    updFace();
}

void TrackSummaryView::tearDownScreen()
{
    TrackSummaryViewBase::tearDownScreen();
}


void TrackSummaryView::setDistance(float km, bool isImperial)
{
    trackSummary.setDistance(km, isImperial);
}

void TrackSummaryView::setSteps(uint32_t v)
{
    trackSummary.setSteps(v);
}

void TrackSummaryView::setAvgPace(uint32_t sec, bool isImperial)
{
    //trackSummary.setAvgPace(sec, isImperial);
}

void TrackSummaryView::setElevation(int32_t m, bool isImperial)
{
    trackSummary.setElevation(m, isImperial);
}

void TrackSummaryView::setTimer(uint32_t sec)
{
    trackSummary.setTimer(sec);
}

void TrackSummaryView::setMaxHR(uint32_t hr)
{
    trackSummaryHR.setMaxHR(hr);
}

void TrackSummaryView::setAvgHR(uint32_t hr)
{
    trackSummaryHR.setAvgHR(hr);
}

void TrackSummaryView::setMap(const TrackMapScreen &map)
{
    trackSummary.setMap(map);
}

void TrackSummaryView::updFace()
{
    trackSummary.setVisible(false);
    trackSummaryHR.setVisible(false);

    if (sideBar.getActiveId() == 1) {
        trackSummaryHR.setVisible(true);
    } else {
        trackSummary.setVisible(true);
    }

    trackSummary.invalidate();
    trackSummaryHR.invalidate();
}

void TrackSummaryView::handleKeyEvent(uint8_t key)
{
    if (key == Gui::Config::Button::L1) {
        mCurrentFace--;
        //sideBar.animateToId(mCurrentFace, Gui::Config::kMenuAnimationSteps);
        sideBar.animateToId(mCurrentFace);
        updFace();

        if (mCurrentFace < 0) {
            mCurrentFace = kFacesNum - 1;
        }
    }

    if (key == Gui::Config::Button::L2) {
        mCurrentFace++;
        //sideBar.animateToId(mCurrentFace, Gui::Config::kMenuAnimationSteps);
        sideBar.animateToId(mCurrentFace);
        updFace();
        if (mCurrentFace >= kFacesNum) {
            mCurrentFace = 0;
        }
    }

    if (key == Gui::Config::Button::R1) {

    }

    if (key == Gui::Config::Button::R2) {
        presenter->exitApp();
    }
}
