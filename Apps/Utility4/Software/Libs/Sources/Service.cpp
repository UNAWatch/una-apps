
#include "Service.hpp"

#define LOG_MODULE_PRX      "Service"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include "SDK/Messages/CommandMessages.hpp"

Service::Service(SDK::Kernel &kernel)
        : mKernel(kernel)
{
    mKernel.sys.delay(100);
}

Service::~Service()
{

}

void Service::run()
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

            case SDK::MessageType::COMMAND_APP_STOP:
                LOG_DEBUG("Stopping...\n");

                // cleanup resources
                mKernel.sys.delay(500);
                // ...

                // We must release message because this is the last event.
                mKernel.comm.releaseMessage(msg);

                // Waiting for the kernel to kill this app
                mKernel.sys.exit(0); // no return

                break;

            case SDK::MessageType::COMMAND_APP_NOTIF_GUI_RUN:
                LOG_DEBUG("GUI is now running\n");
                break;

            case SDK::MessageType::COMMAND_APP_NOTIF_GUI_STOP:
                LOG_DEBUG("GUI has stopped\n");

                // We must release message because this is the last event.
                mKernel.comm.releaseMessage(msg);

                // Waiting for the kernel to kill this app
                mKernel.sys.exit(0); // no return
                break;


            default:
                break;
        }

        // Release message after processing
        mKernel.comm.releaseMessage(msg);
    }

}

