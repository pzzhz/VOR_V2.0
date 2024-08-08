#include "control_OVAR.h"
#include "control_Hardware_API.h"
#include "../other/system_function.h"
#include "control.h"
#include "stdint.h"
#define use_windows
#ifndef STM32F40_41xxx
#include "Windows.h"
#include <stdio.h>
#else
#include "../implement/Slave_OVAR_Ctrl.h"
#endif // use_windows
// 无UI控制
// 返回message

struct
{
    uint32_t time;
    uint32_t SetSec;
    float angle;
    float Vel;
} ovar_info;

uint8_t hal_Slave_OVAR_Init(Task_Parameter_Struct *e)
{
#ifndef STM32F40_41xxx
    ovar_info.time = ControlGetTick();
    ovar_info.SetSec = e->CONT.Sec;
#else

    ovar_info.Vel = e->OVAR.Vel;
    ovar_info.angle = e->OVAR.Inc_Degree;
    if (e->mode == Task_OVAR)
        OVAR_Machine_Init(e->OVAR.Vel, e->OVAR.Sec);
#endif // !STM32F40_41xxx
    return 1;
}

uint8_t hal_Slave_OVAR_Get_State(uint32_t *remainingSec)
{
#ifndef STM32F40_41xxx
    uint32_t currentSec = (ControlGetTick() - ovar_info.time) / 1000.0f;
    if (currentSec < ovar_info.SetSec)
    {
        *remainingSec = ovar_info.SetSec - currentSec;
        return 1;
    }
    *remainingSec = 0;
    return 0;
#else
    uint32_t counterReq = 0, CurrentCount = 0;
    uint8_t res = OVAR_Machine_Get_Count(&counterReq, &CurrentCount);
    *remainingSec = counterReq - CurrentCount;
    return res;
#endif // !STM32F40_41xxx
}

uint8_t hal_Slave_OVAR_Stop(void)
{
#ifndef STM32F40_41xxx
	uint32_t currentCount = vor_info.Freq * (ControlGetTick() - vor_info.time) / 1000.0f;
	vor_info.SetCount = currentCount + 1;
#else
	OVAR_Machine_Stop();
#endif // !STM32F40_41xxx
	return 1;
}

void Inc_handle(float angle)
{
    HAL_Incline_Init(1, angle);
    uint8_t inc_flag = 0;

    while (inc_flag == 0)
    {
        float current_angle;
        inc_flag = HAL_Incline_Get_State(&current_angle);
        MYPRINTF("%.3f", current_angle);
        SaftExitDelay(10, 0);
        MYPRINTF("\r");
    }
}

static void motor_handle(Task_Parameter_Struct *task, Task_control_info *e)
{
    uint8_t OVAR_flag = 1;
    int32_t LastCount = -1;
    uint32_t begin_tick = ControlGetTick();
    uint32_t counter_up = ControlGetTick() - begin_tick;
    uint8_t CAM_State = HAL_CAM_REC_Set(1);
    if (CAM_State == 0)
    {
        Message_Center_Send_prinft(
            "PAGE1", 0,
            0,
            "CAM_ERROR");
        // Message_Center_Send("PAGE1", 0, "CAM_ERROR", sizeof("CAM_ERROR") - 1);
    }
    hal_Slave_OVAR_Init(task);
    MYPRINTF("\r\n");
    while (OVAR_flag)
    {
        uint32_t count;
        OVAR_flag = hal_Slave_OVAR_Get_State(&count);
        if (LastCount != count)
        {
            Message_Center_Send_prinft(
                "PAGE1", 0,
                0,
                "Count:%d", count);
            LastCount = count;
        }
        if (e->State_Bit.Exit) // for exit
        {
            Message_Center_Send_prinft(
                "PAGE1", 0,
                0,
                "Stopping %d", count);
            hal_Slave_OVAR_Stop();
        }
        MYPRINTF("%3d", count);
        // wait motor infinsh
        SaftExitDelay(10, 0);
        MYPRINTF("\r");
    }
    // counter_up = ControlGetTick() - begin_tick;
    CAM_State = HAL_CAM_REC_Set(0);
}

uint8_t OvarControlFunction(Task_Parameter_Struct *task,
                            Task_control_info *e,
                            uint8_t isretract)
{
    MYPRINTF("\r\n ovar begin");
    MYPRINTF("\r\n");
    // 执行部分
    MYPRINTF("\r\n");
    e->UI_para.state = taskruning;
    Inc_handle(task->OVAR.Inc_Degree);
    motor_handle(task, e);
    if (isretract == 1)
        Inc_handle(0);
    MYPRINTF("\r\n ovar end");
    e->UI_para.state = end;
    return 0;
}