#include <gui/trackaction_screen/TrackActionView.hpp>
#include <gui/trackaction_screen/TrackActionPresenter.hpp>

TrackActionPresenter::TrackActionPresenter(TrackActionView& v)
    : view(v)
{

}

void TrackActionPresenter::activate()
{
    view.setUnitsImperial(model->isUnitsImperial());
    onTrackInfo(model->getTrackInfo());

    // Reset idle timer
    model->resetIdleTimer();

    view.setPositionId(model->getMenuPosTrackAction());
}

void TrackActionPresenter::deactivate()
{
    model->setMenuPosTrackAction(view.getPositionId());
}

void TrackActionPresenter::onTrackInfo(const Gui::TrackInfo &info)
{
    view.setTimer(static_cast<uint32_t>(info.totalTime));
    view.setAvgPace(info.avgPace);
    view.setDistance(info.totalDistance);
    view.setSteps(info.steps);
    view.setAvgHR(info.avgHR);
    view.setCalories(info.floors);
}
