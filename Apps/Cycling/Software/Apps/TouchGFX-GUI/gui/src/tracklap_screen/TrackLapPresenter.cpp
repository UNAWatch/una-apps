#include <gui/tracklap_screen/TrackLapView.hpp>
#include <gui/tracklap_screen/TrackLapPresenter.hpp>

TrackLapPresenter::TrackLapPresenter(TrackLapView& v)
    : view(v)
{

}

void TrackLapPresenter::activate()
{
    // Reset idle timer
    model->resetIdleTimer();

    view.setGpsFix(model->getGpsFix());
    view.setUnitsImperial(model->isUnitsImperial());

    const Track::Data& data = model->getTrackData();

    view.setLapNum(data.lapNum);
    view.setSpeed(data.avgLapSpeed);
    view.setDistance(data.lapDistance);
    view.setTimer(data.lapTime);
}

void TrackLapPresenter::deactivate()
{
}

void TrackLapPresenter::onGpsFix(bool acquired)
{
    view.setGpsFix(acquired);
}
