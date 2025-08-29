#include "Interfaces/IModel.hpp"

#include "SDK/CircularBuffer.hpp"

class Model : public IGUIModel
{
public:
    Model(const IKernel& kernel, IServiceModelHandler& handler);
    virtual ~Model();

    void checkG2SEvents();
    bool sendToGUI(const S2GEvent::Data& data);

    void checkS2GEvents()                          override;
    bool sendToService(const G2SEvent::Data& data) override;

private:
    const IKernel&                     mServiceKernel;
    CircularBuffer<S2GEvent::Data, 20> mS2GQueue;
    CircularBuffer<G2SEvent::Data, 20> mG2SQueue;
    IServiceModelHandler&              mServiceHandler;
};
