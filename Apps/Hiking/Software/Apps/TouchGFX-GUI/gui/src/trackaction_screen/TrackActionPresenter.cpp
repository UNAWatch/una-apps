#include <gui/trackaction_screen/TrackActionView.hpp>
#include <gui/trackaction_screen/TrackActionPresenter.hpp>

TrackActionPresenter::TrackActionPresenter(TrackActionView& v)
    : view(v)
{

}

void TrackActionPresenter::activate()
{
    view.setUnitsImperial(model->isUnitsImperial());
    onTrackData(model->getTrackData());

    // Reset idle timer
    model->resetIdleTimer();
    view.setGpsFix(model->getGpsFix());
    view.setPositionId(model->getMenuPosTrackAction());

    model->trackPause();
}

void TrackActionPresenter::deactivate()
{
    model->setMenuPosTrackAction(view.getPositionId());
}

void TrackActionPresenter::onTrackData(const Track::Data& data)
{
    view.setTimer(data.totalTime);
    view.setAvgPace(data.avgPace);
    view.setDistance(data.distance);
    view.setSteps(data.steps);
    view.setAvgHR(data.avgHR);
    view.setFloors(data.floors);
}

void TrackActionPresenter::resumeTrack()
{
    model->trackResume();
}

void TrackActionPresenter::onGpsFix(bool acquired)
{
    view.setGpsFix(acquired);
}
