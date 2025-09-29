#include <gui/tracksummary_screen/TrackSummaryView.hpp>
#include <gui/tracksummary_screen/TrackSummaryPresenter.hpp>

TrackSummaryPresenter::TrackSummaryPresenter(TrackSummaryView& v)
    : view(v)
{

}

void TrackSummaryPresenter::activate()
{
    // Reset idle timer
    model->resetIdleTimer();

    view.setMap(model->trackSummary().map);
    view.setDistance(model->trackSummary().distance, model->isUnitsImperial());
    view.setSteps(model->trackSummary().steps);
    view.setAvgPace(model->trackSummary().paceAvg, model->isUnitsImperial());
    view.setElevation(model->trackSummary().elevation, model->isUnitsImperial());
    view.setTimer(static_cast<uint32_t>(model->trackSummary().time));
    view.setMaxHR(model->trackSummary().hrMax);
    view.setAvgHR(model->trackSummary().hrAvg);
}

void TrackSummaryPresenter::deactivate()
{

}

void TrackSummaryPresenter::exitApp()
{
    if (model->getMenuPosEnterMenu() == App::Menu::ID_LAST_ACTIVITY) {
        model->application().gotoEnterMenuScreenNoTransition();
    } else {
        model->exitApp();
    }
}