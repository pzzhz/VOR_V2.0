/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-22 16:00:07
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-08-08 11:11:54
 * @FilePath: \USERd:\workfile\项目3 vor\software\VOR_V2.0\INTERFACE\UI\control\control_VOR.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "control_VOR.h"
#include "control_Hardware_API.h"
#include "../other/system_function.h"
#include "control.h"
#include "stdint.h"
#define use_windows
#ifndef STM32F40_41xxx
#include "Windows.h"
#include <stdio.h>
#else
#include "../implement/Slave_Vor_Ctrl.h"
#endif // use_windows
	   // 无UI控制
// 返回message

struct
{
	uint32_t time;
	uint32_t SetCount;
	float Vel;
	float Freq;
} vor_info;

uint8_t HAL_Slave_VOR_Init(Task_Parameter_Struct *e)
{
#ifndef STM32F40_41xxx
	vor_info.time = ControlGetTick();
	vor_info.Freq = e->VOR.Freq;
	vor_info.SetCount = e->VOR.Counter;
#else
	if (e->mode == Task_VOR)
		VOR_Machine_Init(e->VOR.Freq, e->VOR.Vel, e->VOR.Counter);
#endif // !STM32F40_41xxx

	return 1;
}

uint8_t HAL_Slave_VOR_Stop(void)
{
#ifndef STM32F40_41xxx
	uint32_t currentCount = vor_info.Freq * (ControlGetTick() - vor_info.time) / 1000.0f;
	vor_info.SetCount = currentCount + 1;
#else
	VOR_Machine_Stop();
#endif // !STM32F40_41xxx
	return 1;
}

uint8_t HAL_Slave_VOR_Get_State(uint32_t *remainingCount)
{
#ifndef STM32F40_41xxx
	uint32_t currentCount = vor_info.Freq * (ControlGetTick() - vor_info.time) / 1000.0f;
	if (currentCount < vor_info.SetCount)
	{
		*remainingCount = vor_info.SetCount - currentCount;
		return 1;
	}
	*remainingCount = 0;
	return 0;
#else
	uint32_t counterReq = 0, CurrentCount = 0;
	uint8_t res = VOR_Machine_Get_Count(&counterReq, &CurrentCount);
	*remainingCount = counterReq - CurrentCount;
	return res;
#endif // !STM32F40_41xxx
}

uint8_t VorControlFunction(Task_Parameter_Struct *task, Task_control_info *e)
{
	// begin noice ui
	uint32_t begin_tick = ControlGetTick();
	uint32_t counter_up = ControlGetTick() - begin_tick;
	e->UI_para.state = ready;
	MYPRINTF("\r\n vor begin");
	MYPRINTF("\r\n");
	// 
	// uint8_t inc_flag = 0;
	// 启动部分
	// while (inc_flag == 0)
	// {
	// 	float current_angle;
	// 	inc_flag = HAL_Incline_Get_State(&current_angle);
	// 	PRINTF("%.3f", current_angle);
	// 	// wait inc and camer
	// 	SaftExitDelay(10, e->ExitFlag);
	// 	/*counter_up = ControlGetTick() - begin_tick;
	// 	e->UI_para.sec = counter_up;*/
	// 	PRINTF("\r");
	// }
	// 执行部分
	MYPRINTF("\r\n");
	HAL_Slave_VOR_Init(task);
	e->UI_para.state = taskruning;
	uint8_t VOR_flag = 1;
	int32_t LastCount = -1;
	uint8_t CAM_State = HAL_CAM_REC_Set(1);
	if (CAM_State == 0)
	{

	}
	uint32_t count;
	// motor initial
	while (VOR_flag)
	{
		VOR_flag = HAL_Slave_VOR_Get_State(&count);
		if (LastCount != count)
		{
			Ctrl_Msg_Printf("%d:VOR Count:%d", e->currentCount, count);
			LastCount = count;
		}
		if (e->State_Bit.Exit)			//for exit
		{
			Ctrl_Msg_Printf("%d:VOR Stopping", e->currentCount);
			HAL_Slave_VOR_Stop();
		}
		MYPRINTF("%3d", count);
		// wait motor infinsh
		SaftExitDelay(50, 0);
		counter_up = ControlGetTick() - begin_tick;
		MYPRINTF("\r");
	}
	CAM_State = HAL_CAM_REC_Set(0);
	MYPRINTF("\r\n vor end");
	e->UI_para.state = end;
}
