/**
 ******************************************************************************
 * @file    main.сpp
 * @date    05-07-2024
 * @author  Denys Saienko <denys.saienko@droid-technologies.com>
 * @brief   Main program body.
 ******************************************************************************
 *
 ******************************************************************************
 */

#include "Service.hpp"

#define LOG_MODULE_PRX      "main::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

#include <cstdint>
#include <cstring>
#include <cassert>
#include <math.h>
#include <stdio.h>


extern const IKernel *kernel;

static uint32_t LoggerGetTicks()
{
    return kernel->app.getTimeMs();
}

static void LoggerPrint(const char* str)
{
    kernel->app.log(str);
}

/**
 * @brief   The application entry point.
 * @retval  int
 */
int main()
{
    Logger_init(LoggerPrint);
    Logger_setTimeFunc(LoggerGetTicks);

    LOG_INFO("started\n");

    Service service(*kernel);
    service.run();
    exit(0);
    return 0;
}

