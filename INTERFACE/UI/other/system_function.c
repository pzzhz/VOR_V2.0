#include "system_function.h"
#ifndef STM32F40_41xxx
#include "Windows.h"
#include <stdio.h>
#else
#include "FreeRTOS.h"
#include "task.h"
#endif // use_windows


void ControlDelay(uint32_t ms)
{
#ifndef STM32F40_41xxx
    Sleep(ms);
#else
    vTaskDelay(ms);
#endif // use_windows
}

void SaftExitDelay(uint32_t ms, uint8_t e)
{
    if (e)
    {
#ifndef STM32F40_41xxx
        ExitThread(0);
#endif // use_windows
    }
#ifndef STM32F40_41xxx
    Sleep(ms);
#else
    vTaskDelay(ms);
#endif // use_windows
}

uint32_t ControlGetTick()
{
#ifndef STM32F40_41xxx
    uint32_t tick = GetTickCount();
    return tick;

#else
    uint32_t tick = xTaskGetTickCount();
    return tick;
#endif
}