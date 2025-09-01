#ifndef __SERVICE_HPP__
#define __SERVICE_HPP__

#include "Interfaces/IKernel.hpp"
#include "GSModel.hpp"


class Service : public IServiceModelHandler,
                public Interface::IUserApp::Callback
{
public:
    Service(const IKernel& kernel);

    virtual ~Service() = default;

    void run();
    virtual void handleEvent(const G2SEvent::Run& event) override;
    virtual void handleEvent(const G2SEvent::Stop& event) override;
    virtual void handleEvent(const G2SEvent::InternalRefresh& event) override;

private:
    void onCreate()  override;
    void onStart()   override;
    void onResume()  override;
    void onStop()    override;
    void onPause()   override;
    void onDestroy() override;

    // Force refresh service state
    void refreshService();

    const IKernel&            mKernel;
    std::shared_ptr<GSModel>  mGSModel;
    bool                      mTerminate;
    bool                      mGUIStarted;

};

#endif
