#ifndef TRACKACTIONVIEW_HPP
#define TRACKACTIONVIEW_HPP

#include <gui_generated/trackaction_screen/TrackActionViewBase.hpp>
#include <gui/trackaction_screen/TrackActionPresenter.hpp>
#include <gui/containers/MenuItemConfig.hpp>

class TrackActionView : public TrackActionViewBase
{
public:
    TrackActionView();
    virtual ~TrackActionView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setPositionId(uint16_t id);
    uint16_t getPositionId();
    void setUnitsImperial(bool isImperial);
    void setTimer(std::time_t sec);
    void setCalories(float calories);
    void setZoneTimes(const std::time_t zoneTimes[5]);
    void setCurrentZone(uint8_t zone);
    void setAvgHR(float hr);

protected:
    using Menu = App::MenuNav::TrackView::Action;

    bool mIsImperial = false;

    float mCalories      = 0.0f;
    std::time_t mZoneTotalSec = 0;
    uint8_t mCurrentZone = 0;
    float mAvgHr         = 0.0f;

    ItemLayout mItemLayout {};

    touchgfx::Callback<TrackActionView, MainMenuItem&, int16_t>       mUpdateItemCb;
    touchgfx::Callback<TrackActionView, MainMenuCenterItem&, int16_t> mUpdateCenterItemCb;
    touchgfx::Callback<TrackActionView, int16_t>                      mCarouselCb;

    void updateItem(MainMenuItem& item, int16_t index);
    void updateCenterItem(MainMenuCenterItem& item, int16_t index);
    void onCarouselUpdate(int16_t index);

    virtual void handleKeyEvent(uint8_t key) override;
};

#endif // TRACKACTIONVIEW_HPP
