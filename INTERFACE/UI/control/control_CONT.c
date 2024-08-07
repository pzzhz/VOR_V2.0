/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-22 16:00:07
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-07-27 18:23:30
 * @FilePath: \USERd:\workfile\项目3 vor\software\VOR_V2.0\INTERFACE\UI\control\control_VOR.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "control_CONT.h"
#include "control_Hardware_API.h"
#include "control.h"
#include "stdint.h"
#define use_windows
#ifndef STM32F40_41xxx
#include "Windows.h"
#include <stdio.h>
#else
#include "../implement/Slave_Cont_Ctrl.h"
#endif // use_windows
       // 无UI控制
// 返回message

struct
{
    uint32_t time;
    uint32_t SetSec;
    float Vel;
} Cont_info;

uint8_t HAL_Slave_CONT_Init(Task_Parameter_Struct *e)
{
#ifndef STM32F40_41xxx
    Cont_info.time = ControlGetTick();
    Cont_info.SetSec = e->CONT.Sec;
#else
    CONT_Machine_Init(e->CONT.Vel, e->CONT.Sec);
#endif // !STM32F40_41xxx

    return 1;
}

uint8_t HAL_Slave_CONT_Stop(void)
{
#ifndef STM32F40_41xxx
    uint32_t currentSec = (ControlGetTick() - Cont_info.time) / 1000.0f;
    Cont_info.SetSec = currentSec + 1;
#else
    Cont_Machine_Stop();
#endif // !STM32F40_41xxx
    return 1;
}

uint8_t HAL_Slave_CONT_Get_State(uint32_t *remainingSec)
{
#ifndef STM32F40_41xxx
    uint32_t currentSec = (ControlGetTick() - Cont_info.time) / 1000.0f;
    if (currentSec < Cont_info.SetSec)
    {
        *remainingSec = Cont_info.SetSec - currentSec;
        return 1;
    }
    *remainingSec = 0;
    return 0;
#else
    uint32_t MillSecReq = 0, CurrentMillSec = 0;
    uint8_t res = Cont_Machine_Get_Count(&MillSecReq, &CurrentMillSec);
    *remainingSec = MillSecReq - CurrentMillSec;
    return res;
#endif // !STM32F40_41xxx
}

uint8_t ContControlFunction(Task_Parameter_Struct *task, Task_control_info *e)
{
    const uint32_t begin_time = 10000;
    // begin noice ui
    uint32_t begin_tick = ControlGetTick();
    uint32_t counter_up = ControlGetTick() - begin_tick;
    e->UI_para.state = ready;
    MYPRINTF("\r\n Cont begin");
    MYPRINTF("\r\n");
    // 执行部分
    MYPRINTF("\r\n");
    HAL_Slave_CONT_Init(task);
    e->UI_para.state = taskruning;
    uint8_t CONT_flag = 1;
    int32_t LastCount = -1;
    // motor initial
    while (CONT_flag)
    {
        uint32_t count;
        CONT_flag = HAL_Slave_CONT_Get_State(&count);
        if (LastCount != count)
        {
            HAL_Set_UI_Page1_Msg("Count:%d", count);
            LastCount = count;
        }
        MYPRINTF("%3d", count);
        // wait motor infinsh
        SaftExitDelay(10, 0);
        counter_up = ControlGetTick() - begin_tick;
        MYPRINTF("\r");
    }
    MYPRINTF("\r\n vor end");
    e->UI_para.state = end;
}
