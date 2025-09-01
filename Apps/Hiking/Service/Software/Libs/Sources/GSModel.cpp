#include "GSModel.hpp"

#include <cassert>

GSModel::GSModel(const IKernel& kernel, IServiceModelHandler& handler)
    : mServiceKernel(kernel)
    , mS2GQueue(kernel)
    , mG2SQueue(kernel)
    , mServiceHandler(handler)
    , mGUIHandler(nullptr)
    , mGUIKernel(nullptr)
{
    mS2GQueue.init();
    mG2SQueue.init();
}

GSModel::~GSModel()
{
}

void GSModel::setGUIHandler(const IKernel* kernel, IGUIModelHandler* handler)
{
    mGUIKernel  = kernel;
    mGUIHandler = handler;
}

S2GEvent::State GSModel::checkS2GEvents(uint32_t timeout)
{
    assert(mGUIKernel != nullptr);
    assert(mGUIHandler != nullptr);

    S2GEvent::Data data {};

    mGUIKernel->app.unLock();
    bool status = mS2GQueue.pop(data, timeout);
    mGUIKernel->app.lock();

    if (status) {
        if (mGUIHandler) {
            std::visit([this](const auto &event) {
                mGUIHandler->handleEvent(event);
            }, data);

            return mGUIHandler->getGUIState();
        }
    }

    return S2GEvent::State::none;
}

bool GSModel::sendToService(const G2SEvent::Data& data)
{
    return mG2SQueue.push(data);
}

void GSModel::updateGUIState(S2GEvent::State state)
{
    sendToGUI(S2GEvent::UpdateState {state});
}

void GSModel::checkG2SEvents(uint32_t timeout)
{
    G2SEvent::Data data {};

    mServiceKernel.app.unLock();
    bool status = mG2SQueue.pop(data, timeout);
    mServiceKernel.app.lock();

    if (!status) {
        return;
    }

    std::visit([this](const auto &event) {
        mServiceHandler.handleEvent(event);
    }, data);
}

bool GSModel::sendToGUI(const S2GEvent::Data& data)
{
    return mS2GQueue.push(data);
}
