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
	uint32_t RemainTime;
	uint8_t flag_pause;
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
	ovar_info.SetSec = e->OVAR.Sec;
	ovar_info.angle = e->OVAR.Inc_Degree;
	if (e->mode == Task_OVAR)
		OVAR_Machine_Init(e->OVAR.Vel, e->OVAR.Sec * 1000);
#endif // !STM32F40_41xxx
	return 1;
}

uint8_t HAL_Slave_OVAR_Pause(uint8_t enable)
{
#ifndef STM32F40_41xxx
	if (enable)
	{
		ovar_info.flag_pause = 1;
		ovar_info.RemainTime = ControlGetTick() - ovar_info.time;
	}
	else
	{
		ovar_info.time = ControlGetTick() - ovar_info.RemainTime;
		ovar_info.flag_pause = 0;
	}
#else
	OVAR_Machine_Pause();
#endif // !STM32F40_41xxx
	return 1;
}

uint8_t hal_Slave_OVAR_Get_State(uint32_t *remainingSec, uint32_t *parcent)
{
#ifndef STM32F40_41xxx
	if (ovar_info.flag_pause)
	{
		return Imp_paused;
	}
	uint32_t currentSec = (ControlGetTick() - ovar_info.time) / 1000.0f;
	if (parcent != 0)
		*parcent = currentSec * 100 / ovar_info.SetSec;
	if (currentSec < ovar_info.SetSec)
	{
		*remainingSec = ovar_info.SetSec - currentSec;
		return Imp_running;
	}
	*remainingSec = 0;
	return Imp_finsih;
#else
	uint32_t counterReq = 0, CurrentCount = 0;
	uint8_t res = OVAR_Machine_Get_Count(&counterReq, &CurrentCount);
	if (parcent != 0)
		*parcent = CurrentCount * 100 / counterReq;
	*remainingSec = counterReq - CurrentCount;
	return res;
#endif // !STM32F40_41xxx
}

uint8_t hal_Slave_OVAR_Stop(void)
{
#ifndef STM32F40_41xxx
	uint32_t currentSec = (ControlGetTick() - ovar_info.time) / 1000.0f;
	ovar_info.SetSec = currentSec + 1;
#else
	OVAR_Machine_Stop();
#endif // !STM32F40_41xxx
	return 1;
}

void Inc_handle(float angle)
{
	HAL_Incline_Init(angle, 0);
	uint8_t inc_flag = 1;

	while (inc_flag)
	{
		float current_angle;
		inc_flag = HAL_Incline_Get_State(&current_angle);
		Ctrl_Msg_Printf("inc angle %d", (int)current_angle);
		MYPRINTF("%.3f", current_angle);
		SaftExitDelay(10, 0);
		MYPRINTF("\r");
	}
}

static void motor_handle(Task_Parameter_Struct *task, Task_control_info *e)
{
	const int camWaitTime_s = 5;
	uint8_t OVAR_flag = 1;
	int32_t LastCount = -1;
	Pause_Resume:
	uint8_t CAM_State = HAL_CAM_REC_Set(1);
	for (int i = 0; i < camWaitTime_s; i++)
	{
		(CAM_State == 0 && i < 2) ? Ctrl_Msg_Printf("camera error") :Ctrl_Msg_Printf("Run after %ds", camWaitTime_s-i);
		SaftExitDelay(1000, 0);
	}
	hal_Slave_OVAR_Init(task);
	MYPRINTF("\r\n");
	uint32_t count, parcent;
	uint8_t pauseFlag = 0, CamIsStop = 0;
	e->State_Bit.reqPause = 0;
	while (OVAR_flag)
	{
		OVAR_flag = hal_Slave_OVAR_Get_State(&count, &parcent);
		if (LastCount != count && OVAR_flag == Imp_running)
		{
			Ctrl_Msg_Printf("%d OVAR Done:%d%%", e->currentCount+1, (int)parcent);
			LastCount = count;
		}
		if (e->State_Bit.Exit) // for exit
		{
			Ctrl_Msg_Printf("%d:OVAR #A52A2A Terminated#", e->currentCount+1);
			e->UI_para.state = taskTerminal;
			hal_Slave_OVAR_Stop();
		}
		if (e->State_Bit.reqPause)
		{
			if (OVAR_flag == Imp_running)
			{
				e->State_Bit.isPause = 1;
				HAL_Slave_OVAR_Pause(1);
				HAL_CAM_REC_Set(1);
				e->UI_para.state = taskPause;
                Rk3588_Send_Pause;
				Ctrl_Msg_Printf("%d:OVAR Pause", e->currentCount+1);
				//				if (CamIsStop == 0)
				//					HAL_CAM_REC_Set(1);
				//				CamIsStop = 1;
			}
			else if (OVAR_flag == Imp_paused)
			{
				e->State_Bit.isPause = 0;
				//e->UI_para.state = taskruning;
				goto Pause_Resume;
			}
			e->State_Bit.reqPause = 0;
		}

		MYPRINTF("%3d", count);
		// wait motor infinsh
		SaftExitDelay(10, 0);
		MYPRINTF("\r");
	}
	if (e->State_Bit.Exit != 1)
	{
		Ctrl_Msg_Printf("%d:OVAR Done:100%%", e->currentCount+1);
	}
	/*one sec for cam stop*/
	SaftExitDelay(1000, 0);
	CAM_State = HAL_CAM_REC_Set(1);
	if (CAM_State == 0)
	{
		Ctrl_Msg_Printf("CAM ERROR");
	}
	SaftExitDelay(1000, 0);
	/*--one sec for cam stop*/
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
	if (isretract == 1||e->State_Bit.Exit)
		Inc_handle(0);
	Ctrl_Msg_Printf("OVAR end ");
	MYPRINTF("\r\n ovar end");
	e->UI_para.state = taskend;
	return 0;
}