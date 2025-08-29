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
#include <cassert>
#include <math.h>
#include <stdio.h>
#include <memory>

#include "SDK/GSModel/GSModelHelper.hpp"

static const char* mName = "Service::Utility#1";

class Service : public IServiceModelHandler,
                public Interface::IUserApp::Callback
{
public:
    Service(const IKernel& kernel)
        : mGSModel(std::make_shared<GSModelService>(kernel, *this))
        , mTerminate(false)
        , mCounter(0)
        , mGUIStarted(false)
    {
        kernel.app.registerApp(this);
        kernel.sctrl.setContext(mGSModel);
        kernel.app.initialized();
    }

    virtual ~Service() = default;

    void run()
    {
        while (!mTerminate) {
            mGSModel->checkG2SEvents();

            mCounter += 1;

            if (mGUIStarted) {
                S2GEvent::Counter counter = {
                    .value = mCounter
                };

                mGSModel->sendToGUI(counter);
            }
        }
    }

    void handleEvent(const G2SEvent::Run& event)
    {
        (void) event;

        printf("%s::G2SEvent::Run\n", mName);

        mGUIStarted = true;
    }

    void handleEvent(const G2SEvent::Stop& event)
    {
        (void) event;

        printf("%s::G2SEvent::Stop\n", mName);

        mGUIStarted = false;
    }

private:
    void onDestroy() override
    {
        mTerminate = true;
    }

    std::shared_ptr<GSModelService> mGSModel;
    bool                            mTerminate;
    uint32_t                        mCounter;
    bool                            mGUIStarted;
};

/**
 * @brief   The application entry point.
 * @retval  int
 */
int main()
{
    printf("%s is started\n", mName);

    extern const IKernel* kernel;

    Service service(*kernel);
    service.run();

    return 0;
}

