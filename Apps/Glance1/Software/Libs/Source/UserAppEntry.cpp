#include "Service.hpp"

#define LOG_MODULE_PRX      "Service::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

void UserAppEntry()
{
    Service service;
    service.run();
}
