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

#include <stdio.h>
#include "SDK/KernelManager.hpp"

#define LOG_MODULE_PRX      "main::"
#define LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#include "SDK/UnaLogger/Logger.h"

extern "C" void touchgfx_init(void);
extern "C" void touchgfx_components_init(void);
extern "C" void touchgfx_taskEntry(void);

extern const IKernel* kernel;

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

    KernelManager::CreateInstance(kernel);

    touchgfx_components_init();
    touchgfx_init();
    touchgfx_taskEntry();   // No return

    return 0;
}
