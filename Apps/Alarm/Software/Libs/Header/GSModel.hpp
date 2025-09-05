#include "Interfaces/IModel.hpp"
#include "SDK/CircularBuffer.hpp"

#include <memory>

class GSModel : public IGUIModel
{
public:
    GSModel(const IKernel& kernel, IServiceModelHandler& handler);
    virtual ~GSModel();

    void checkG2SEvents(uint32_t timeout = 0xFFFFFFFF);
    bool sendToGUI(const S2GEvent::Data& data);

    // IGUIModel interface
    void            setGUIHandler(const IKernel* kernel, IGUIModelHandler* handler) override;
    S2GEvent::State checkS2GEvents(uint32_t timeout)                                override;
    bool            sendToService(const G2SEvent::Data& data)                       override;
    void            updateGUIState(S2GEvent::State state)                           override;

private:
    const IKernel&                     mServiceKernel;
    CircularBuffer<S2GEvent::Data, 10> mS2GQueue;
    CircularBuffer<G2SEvent::Data, 10> mG2SQueue;
    IServiceModelHandler&              mServiceHandler;
    IGUIModelHandler*                  mGUIHandler;
    const IKernel*                     mGUIKernel;
};
