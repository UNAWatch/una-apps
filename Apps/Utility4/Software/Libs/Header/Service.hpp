
#pragma once

#include "SDK/Kernel/Kernel.hpp"

class Service {

public:

    Service(SDK::Kernel &kernel);

    virtual ~Service();

    void run();

private:
    const SDK::Kernel &mKernel;

};
