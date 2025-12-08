
#include "Gui.hpp"

#define LOG_MODULE_PRX      "Gui"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include "SDK/Messages/CommandMessages.hpp"

static uint8_t frameBuffer[240*240];
static bool isResumed;

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

            case SDK::MessageType::COMMAND_APP_GUI_RESUME:
                LOG_DEBUG("Resume\n");
                msg->setResult(SDK::MessageResult::SUCCESS);
                mKernel.comm.sendResponse(msg);
                isResumed = true;
                break;

            case SDK::MessageType::COMMAND_APP_GUI_SUSPEND:
                LOG_DEBUG("Suspend\n");
                msg->setResult(SDK::MessageResult::SUCCESS);
                mKernel.comm.sendResponse(msg);
                isResumed = false;
                break;

            case SDK::MessageType::EVENT_GUI_TICK: {
                LOG_DEBUG("Frame tick\n");
                msg->setResult(SDK::MessageResult::SUCCESS);
                mKernel.comm.sendResponse(msg);

                if (isResumed) {
                    auto* updDisp = mKernel.comm.allocateMessage<SDK::Message::RequestDisplayUpdate>();
                    if (updDisp) {
                        memset(frameBuffer, (3 << 4), sizeof(frameBuffer));
                        updDisp->pBuffer = frameBuffer;
                        mKernel.comm.sendMessage(updDisp, 0);
                        mKernel.comm.releaseMessage(updDisp);
                    }
                }
            } break;

            case SDK::MessageType::EVENT_BUTTON: {
                auto *button = static_cast<SDK::Message::EventButton*>(msg);
                LOG_DEBUG("Button event 0x%08X. Id %d, Event %d\n", msg->getType(), button->id, button->event);
                if (button->id == SDK::Message::EventButton::Id::SW1) {
                    auto* appTerm = mKernel.comm.allocateMessage<SDK::Message::RequestAppTerminate>();
                    if (appTerm) {
                        appTerm->code = 0;
                        mKernel.comm.sendMessage(appTerm, 0);
                        mKernel.comm.releaseMessage(appTerm);

                        // Waiting for the kernel to kill this app
                        while (true) {
                            mKernel.sys.delay(1000);
                        }
                    }
                }

                if (button->id == SDK::Message::EventButton::Id::SW2 && button->event == SDK::Message::EventButton::Event::CLICK) {
                    auto* bl = mKernel.comm.allocateMessage<SDK::Message::RequestBacklightSet>();
                    if (bl) {
                        bl->brightness = 100;
                        bl->autoOffTimeoutMs = 1000;
                        mKernel.comm.sendMessage(bl, 0);
                        mKernel.comm.releaseMessage(bl);
                    }
                }

            } break;

            default:
                break;
        }

        // Release message after processing
        mKernel.comm.releaseMessage(msg);
    }

}

