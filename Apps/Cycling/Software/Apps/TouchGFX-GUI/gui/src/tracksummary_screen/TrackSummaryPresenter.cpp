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

    view.setDistance(model->trackSummary().distance, model->isUnitsImperial());
    view.setAvgSpeed(model->trackSummary().speedAvg, model->isUnitsImperial());
    view.setTimer(static_cast<uint32_t>(model->trackSummary().time));
    view.setMaxHR(model->trackSummary().hrMax);
    view.setAvgHR(model->trackSummary().hrAvg);
    view.setMap(model->trackSummary().map);
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
