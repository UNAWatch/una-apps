/**
 ******************************************************************************
 * @file    main.сpp
 * @date    05-07-2024
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Main program body.
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <cstdint>
#include <cstring>
#include <math.h>
#include <stdio.h>

#include "SDK/GSModel/IGUIModel.hpp"
#include "SDK/GSModel/GSModelHelper.hpp"

extern const IKernel *kernel;

static const char* mName = "GUI::Utility#1";

class Application : public IGUIModelHandler,
                    public Interface::IUserApp::Callback
{
public:
    Application(const IKernel& kernel)
        : mGSModel(std::static_pointer_cast<GSModelGUI>(kernel.gctrl.getContext()))
        , mTerminate(false)
    {
        kernel.app.registerApp(this);
        mGSModel->setGUIHandler(&kernel, this);
        kernel.app.initialized();
    }

    virtual ~Application()
    {
        mGSModel->sendToService(G2SEvent::Stop {});
        mGSModel->setGUIHandler(nullptr, nullptr);
    }

    void run()
    {
        mGSModel->sendToService(G2SEvent::Run {});

        while (!mTerminate) {
            mGSModel->checkS2GEvents();
        }
    }

    void handleEvent(const S2GEvent::Counter& event) override
    {
        printf("%s::counter = %ld\n", mName, event.value);
    }

private:
    void onCreate() override
    {
        printf("%s::onCreate()\n", mName);
    }

    void onStart() override
    {
        printf("%s::onStart()\n", mName);
    }

    virtual void onResume() override
    {
        printf("%s::onResume()\n", mName);
    }

    void onPause() override
    {
        printf("%s::onPause()\n", mName);
    }

    void onStop() override
    {
        printf("%s::onStop()\n", mName);
    }

    void onDestroy() override
    {
        printf("%s::onDestroy()\n", mName);
        mTerminate = true;
    }

    std::shared_ptr<GSModelGUI> mGSModel;
    volatile bool               mTerminate;
};

/**
 * @brief   The application entry point.
 * @retval  int
 */
int main()
{
    printf("%s is started\n", mName);

    Application app(*kernel);
    app.run();

    return 0;
}
