#ifndef MAINVIEW_HPP
#define MAINVIEW_HPP

#include <gui_generated/main_screen/MainViewBase.hpp>
#include <gui/main_screen/MainPresenter.hpp>

class MainView : public MainViewBase
{
public:
    MainView();
    virtual ~MainView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void updateAlarmList(const std::vector<AppType::Alarm>& list);
    void setActiveAlarm(size_t id);

protected:

    size_t mAlarmId = 0; // Current alarm index. If equals to pList->size() -> New Alarm
    const std::vector<AppType::Alarm>* pList = nullptr;

    virtual void handleKeyEvent(uint8_t key) override;

    void show();
};

#endif // MAINVIEW_HPP
