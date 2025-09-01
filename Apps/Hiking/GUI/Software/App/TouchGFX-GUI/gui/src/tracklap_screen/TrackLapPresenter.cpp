#include <gui/tracklap_screen/TrackLapView.hpp>
#include <gui/tracklap_screen/TrackLapPresenter.hpp>

TrackLapPresenter::TrackLapPresenter(TrackLapView& v)
    : view(v)
{

}

void TrackLapPresenter::activate()
{
    view.setUnitsImperial(model->isUnitsImperial());
    onTrackInfo(model->getTrackInfo());

    // Reset idle timer
    model->resetIdleTimer();
}

void TrackLapPresenter::deactivate()
{

}

void TrackLapPresenter::onTrackInfo(const Gui::TrackInfo &info)
{
    view.setLapNum(info.lapNum);
    view.setDistance(info.totalDistance);
    view.setTimer(static_cast<uint32_t>(info.totalTime));
    view.setSteps(info.steps);
}
