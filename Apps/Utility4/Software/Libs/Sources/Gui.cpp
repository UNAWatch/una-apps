
#include "Gui.hpp"

#define LOG_MODULE_PRX      "Gui"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include "SDK/Messages/CommandMessages.hpp"

Gui::Gui(SDK::Kernel &kernel)
        : mKernel(kernel)
{
    mKernel.sys.delay(100);
}

Gui::~Gui()
{

}

void Gui::run()
{
    mKernel.sys.delay(500);

    LOG_DEBUG("Ready\n");

    while (true) {
        SDK::MessageBase *msg;

        // Wait for command (blocks until available)
        if(!mKernel.comm.getMessage(msg)) {
            continue;
        }

        switch (msg->getType()) {
            case SDK::MessageType::COMMAND_APP_RUN:
                LOG_DEBUG("Initializing...\n");

                // confirm starting
                msg->setResult(SDK::MessageResult::SUCCESS);
                mKernel.comm.sendResponse(msg);
                break;

            case SDK::MessageType::COMMAND_APP_STOP:
                LOG_DEBUG("Stopping...\n");

                // cleanup resources
                mKernel.sys.delay(500);
                // ...

                // confirm stopping
                msg->setResult(SDK::MessageResult::SUCCESS);
                mKernel.comm.sendResponse(msg);

                // We must release message because this is the last event.
                mKernel.comm.releaseMessage(msg);

                // Waiting for the kernel to kill this app
                while (true) {
                    mKernel.sys.delay(1000);
                }

                break;

            case SDK::MessageType::EVENT_GUI_FRAME_TICK:
                LOG_DEBUG("Frame tick\n");
                break;

            case SDK::MessageType::COMMAND_APP_GUI_RESUME:
                LOG_DEBUG("Resume\n");
                msg->setResult(SDK::MessageResult::SUCCESS);
                mKernel.comm.sendResponse(msg);
                break;

            case SDK::MessageType::COMMAND_APP_GUI_SUSPEND:
                LOG_DEBUG("Suspend\n");
                break;

            default:
                break;
        }

        // Release message after processing
        mKernel.comm.releaseMessage(msg);
    }

}

