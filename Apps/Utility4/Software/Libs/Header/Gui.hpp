
#pragma once

#include "SDK/Kernel/Kernel.hpp"

class Gui {

public:

    Gui(SDK::Kernel &kernel);

    virtual ~Gui();

    void run();

private:
    const SDK::Kernel &mKernel;

};
