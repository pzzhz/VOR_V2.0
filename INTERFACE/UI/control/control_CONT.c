/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-22 16:00:07
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @LastEditTime: 2025-01-17 14:19:50
 * @FilePath: \USERd:\workfile\项目3 vor\software\VOR_V2.0\INTERFACE\UI\control\control_VOR.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "control_CONT.h"
#include "control_Hardware_API.h"
#include "../other/system_function.h"
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
	uint32_t RemainTime;
	uint8_t flag_pause;
	uint32_t SetSec;
	float Vel;
} Cont_info;

uint8_t HAL_Slave_CONT_Init(Task_Parameter_Struct* e)
{
#ifndef STM32F40_41xxx
	Cont_info.time = ControlGetTick();
	Cont_info.SetSec = e->CONT.Sec;
#else
	CONT_Machine_Init(e->CONT.Vel, e->CONT.Sec * 1000);
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

uint8_t HAL_Slave_CONT_Pause(uint8_t enable)
{
#ifndef STM32F40_41xxx
	if (enable)
	{
		Cont_info.flag_pause = 1;
		Cont_info.RemainTime = ControlGetTick() - Cont_info.time;
	}
	else
	{
		Cont_info.time = ControlGetTick() - Cont_info.RemainTime;
		Cont_info.flag_pause = 0;
	}
#else
	Cont_Machine_Pause();
#endif // !STM32F40_41xxx
	return 1;
}


uint8_t HAL_Slave_CONT_Get_State(uint32_t* remainingSec, uint32_t* parcent)
{
#ifndef STM32F40_41xxx
	if (Cont_info.flag_pause)
		return Imp_pause;
	uint32_t currentSec = (ControlGetTick() - Cont_info.time) / 1000.0f;
	if (parcent != 0)
		*parcent = currentSec * 100 / Cont_info.SetSec;
	if (currentSec < Cont_info.SetSec)
	{
		*remainingSec = Cont_info.SetSec - currentSec;
		return  Imp_running;
	}
	*remainingSec = 0;
	return  Imp_finsih;
#else
	uint32_t MillSecReq = 0, CurrentMillSec = 0;
	uint8_t res = Cont_Machine_Get_Count(&MillSecReq, &CurrentMillSec);
	if (parcent != 0 && MillSecReq != 0)
		*parcent = CurrentMillSec * 100 / MillSecReq;
	*remainingSec = MillSecReq - CurrentMillSec;
	return res;
#endif // !STM32F40_41xxx
}

uint8_t ContControlFunction(Task_Parameter_Struct* task, Task_control_info* e)
{
	const uint32_t begin_time = 10000;
	const uint32_t camWaitTime_s = 5;

	e->UI_para.state = ready;
	MYPRINTF("\r\n Cont begin");
	MYPRINTF("\r\n");
	// 执行部分
	MYPRINTF("\r\n");
	/*cam rec*/
	uint8_t CAM_State = HAL_CAM_REC_Set(1);
	for (int i = 0; i < camWaitTime_s; i++)
	{
		(CAM_State == 0 && i < 2) ?
			Ctrl_Msg_Printf("camera error") :
			Ctrl_Msg_Printf("start after %ds", camWaitTime_s - i);
		SaftExitDelay(1000, 0);
	}
	/*--cam rec*/
	/*begin motion*/
	HAL_Slave_CONT_Init(task);
	e->UI_para.state = taskruning;
	uint8_t CONT_flag = 1, pauseFlag = 0, CamIsStop = 0;	//防止启动时候发生意外触发
	int32_t LastCount = -1;
	// motor initial
	uint32_t count, parcent;
	e->State_Bit.pause = 0;
	while (CONT_flag)
	{
		CONT_flag = HAL_Slave_CONT_Get_State(&count, &parcent);
		if (LastCount != count && e->State_Bit.pause == 0)
		{
			Ctrl_Msg_Printf("%d:CONT Done:%d%%", e->currentCount, parcent);
			// HAL_Set_UI_Page1_Msg("Count:%d", count);
			LastCount = count;
		}
		if (e->State_Bit.Exit) // for exit
		{
			Ctrl_Msg_Printf("%d:CONT #A52A2A Terminated#", e->currentCount);
			HAL_Slave_CONT_Stop();
		}
		if (e->State_Bit.pause != pauseFlag)
		{
			if (e->State_Bit.pause)
			{
				HAL_Slave_CONT_Pause(1);
				Ctrl_Msg_Printf("%d:CONT Pause", e->currentCount);
				//				if (CamIsStop == 0)
				//					HAL_CAM_REC_Set(1);
				//				CamIsStop = 1;
			}
			else
			{
				HAL_Slave_CONT_Pause(0);
			}
		}
		pauseFlag = e->State_Bit.pause;
		MYPRINTF("%3d", count);
		// wait motor infinsh
		SaftExitDelay(10, 0);

		MYPRINTF("\r");
	}
	Ctrl_Msg_Printf("%d:CONT Done:100%%", e->currentCount);
	/*one sec for cam stop*/
	SaftExitDelay(1000, 0);
	if (CamIsStop == 0)
		HAL_CAM_REC_Set(1);
	/*--one sec for cam stop*/
	Ctrl_Msg_Printf("end");
	MYPRINTF("\r\n vor end");
	e->UI_para.state = end;
}
