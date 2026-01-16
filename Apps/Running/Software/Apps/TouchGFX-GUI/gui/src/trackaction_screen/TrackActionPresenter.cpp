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
    view.setAvgHR(data.avgHR);
    view.setElevation(data.elevation);
}

void TrackActionPresenter::onGpsFix(bool acquired)
{
    view.setGpsFix(acquired);
}
