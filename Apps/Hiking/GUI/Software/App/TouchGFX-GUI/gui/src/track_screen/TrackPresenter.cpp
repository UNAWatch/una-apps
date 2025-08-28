#include <gui/track_screen/TrackView.hpp>
#include <gui/track_screen/TrackPresenter.hpp>

TrackPresenter::TrackPresenter(TrackView& v)
    : view(v)
{

}

void TrackPresenter::activate()
{
    // Reset nested menu position
    model->setMenuPosTrackAction(0);

    // Set current menu position
    view.setPositionId(model->getMenuPosTrack());

    onTrackInfo(model->getTrackInfo());

    uint8_t hour;
    uint8_t minute;
    uint8_t sec;
    model->getTime(hour, minute, sec);
    view.setTime(hour, minute);

    view.setBatteryLevel(model->getBatteryLevel());
}

void TrackPresenter::deactivate()
{
    model->setMenuPosTrack(view.getPositionId());
}

void TrackPresenter::onTrackInfo(const Gui::TrackInfo &info)
{
    view.setTrackInfo(info, model->isUnitsImperial());
}

void TrackPresenter::onBatteryLevel(uint8_t lvl)
{
    view.setBatteryLevel(lvl);
}

void TrackPresenter::onTime(uint8_t hour, uint8_t minute, uint8_t sec)
{
    view.setTime(hour, minute);
}

void TrackPresenter::onLapChanged()
{
    model->application().gotoTrackLapScreenNoTransition();
}
