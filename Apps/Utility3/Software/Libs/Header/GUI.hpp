#ifndef __GUI_HPP__
#define __GUI_HPP__

#include "SDK/GSModel/GSModelHelper.hpp"
#include "SDK/Kernel/Kernel.hpp"

class GUI : public IGUIModelHandler,
            public SDK::Interface::IUserApp::Callback
{
public:
    GUI();
    ~GUI();

    void run();

    void handleEvent(const S2GEvent::Counter& event) override;

private:
    void onCreate()  override;
    void onStart()   override;
    void onResume()  override;
    void onPause()   override;
    void onStop()    override;
    void onDestroy() override;

    const SDK::Kernel           mKernel;
    std::shared_ptr<GSModelGUI> mGSModel;
    volatile bool               mTerminate;
};

#endif
