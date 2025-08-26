#ifndef DELETEDVIEW_HPP
#define DELETEDVIEW_HPP

#include <gui_generated/deleted_screen/DeletedViewBase.hpp>
#include <gui/deleted_screen/DeletedPresenter.hpp>

class DeletedView : public DeletedViewBase
{
public:
    DeletedView();
    virtual ~DeletedView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void setAlarmId(size_t id);
protected:

    uint16_t mCounter = Gui::Config::kConfirmTimeout;
    virtual void handleTickEvent() override;
};

#endif // DELETEDVIEW_HPP
