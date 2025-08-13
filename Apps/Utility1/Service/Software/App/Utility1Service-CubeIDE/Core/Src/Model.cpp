#include "../Inc/Model.hpp"

#include <assert.h>

Model::Model(const IKernel& kernel, IServiceModelHandler& handler)
    : mServiceKernel(kernel)
    , mS2GQueue(kernel)
    , mG2SQueue(kernel)
    , mServiceHandler(handler)
{
    mS2GQueue.init();
    mG2SQueue.init();
}

Model::~Model()
{
}

void Model::checkS2GEvents()
{
    S2GEvent::Data data {};

    mGUIKernel->app.unLock();
    bool status = mS2GQueue.pop(data, 1000);
    mGUIKernel->app.lock();

    if (!status) {
        return;
    }

    if (mGUIHandler) {
        std::visit([this](const auto &event) {
            mGUIHandler->handleEvent(event);
        }, data);
    }
}

bool Model::sendToService(const G2SEvent::Data& data)
{
    return mG2SQueue.push(data);
}

void Model::checkG2SEvents()
{
    G2SEvent::Data data {};

    mServiceKernel.app.unLock();
    bool status = mG2SQueue.pop(data, 1000);
    mServiceKernel.app.lock();

    if (!status) {
        return;
    }

    std::visit([this](const auto &event) {
        mServiceHandler.handleEvent(event);
    }, data);
}

bool Model::sendToGUI(const S2GEvent::Data& data)
{
    return mS2GQueue.push(data);
}
