#include "Service.hpp"

#define LOG_MODULE_PRX      "UserAppEntry::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

void UserAppEntry()
{
    Service service;
    service.run();
}
