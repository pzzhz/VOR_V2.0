#include "control_VOR.h"
#include "control.h"
#include "stdint.h"
#define use_windows
#ifndef STM32F40_41xxx
#include "Windows.h"
#include <stdio.h>
#endif // use_windows
//无UI控制
//返回message


void VorControlFunction(Mode_info_struct* e)
{
    const uint32_t begin_time = 1000;
    //begin noice ui
    uint32_t begin_tick = ControlGetTick();
    uint32_t counter_up = ControlGetTick() - begin_tick;
    e->UI_para.state = ready;
    //启动部分
    while (counter_up < begin_time)
    {
        //wait inc and camer
        SaftExitDelay(10, e);
        counter_up = ControlGetTick() - begin_tick;
        e->UI_para.sec = counter_up;
    }

    //执行部分
    begin_tick = ControlGetTick();
    counter_up = ControlGetTick() - begin_tick;
    e->UI_para.state = taskruning;
    //motor initial
    while (counter_up < begin_time * 10)
    {
        //wait motor infinsh
        SaftExitDelay(10, e);
        counter_up = ControlGetTick() - begin_tick;
    }
    e->UI_para.state = end;
    e->FinishFlag = TRUE;
}
