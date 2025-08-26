#ifndef SAVEDVIEW_HPP
#define SAVEDVIEW_HPP

#include <gui_generated/saved_screen/SavedViewBase.hpp>
#include <gui/saved_screen/SavedPresenter.hpp>

class SavedView : public SavedViewBase
{
public:
    SavedView();
    virtual ~SavedView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setAlarmId(size_t id);
protected:

    uint16_t mCounter = Gui::Config::kConfirmTimeout;
    virtual void handleTickEvent() override;
};

#endif // SAVEDVIEW_HPP
