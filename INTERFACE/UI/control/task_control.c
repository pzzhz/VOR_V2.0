/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-24 10:26:50
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-07-27 10:40:18
 * @FilePath: \USERd:\workfile\项目3 vor\software\VOR_V2.0\INTERFACE\UI\control\task_control.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "task_control.h"
#include "../other/system_function.h"
#include "../Task/task_info_struct.h"
#include "control_Hardware_API.h"
#include "stdint.h"

#ifndef STM32F40_41xxx
#include "Windows.h"
#include <stdio.h>
#else
#include "FreeRTOS.h"
#include "task.h"
#endif

#include "control_VOR.h"
#include "control_CONT.h"

Task_Parameter_Struct TaskArray[10];
typedef struct
{
	uint16_t currentTaskIndex;
	uint32_t remainingTime;
} Task_Control_Status;
static char msg_buffer[50];

void task_interval_handle(int index)
{
	const uint32_t waitMillSec = 5000;
	uint32_t time = ControlGetTick(), remainingTime = 0;
	//uint16_t len = sprintf(msg_buffer, "Interval: ID:%d", index);
	//Message_Center_Send("PAGE1", 0, msg_buffer, len);
	Message_Center_Send_prinft(
		"PAGE1", 0,
		"Interval: ID:%d", index);
	/*HAL_Set_UI_Page1_Msg("Interval: ID:%d", index);*/
	while (remainingTime < waitMillSec)
	{
		remainingTime = ControlGetTick() - time;
		Message_Center_Send_prinft(
			"PAGE1", 0,
			"Interval: Sec:%d", waitMillSec - remainingTime);
		/*len = sprintf(msg_buffer, "Interval: Sec:%d", waitMillSec - remainingTime);
		Message_Center_Send("PAGE1", 0, msg_buffer, len);*/
		//HAL_Set_UI_Page1_Msg("Interval: Sec:%d", waitMillSec - remainingTime);
		ControlDelay(100);
	}
}

uint8_t Task_control_Begin(Task_control_info* e)
{
	if (e->State_Bit.IsRunning == 1)
	{
		return 0;
	}
	e->taskCount = HAL_Task_GetArray(TaskArray, 10);
	if (e->taskCount == 0 || e->taskCount > 20)
		return 0;
	e->taskArray = TaskArray;
	e->State_Bit.Init = 1;
	return 1;
}

void Task_control_handler(Task_control_info* e)
{
	if (e == 0)
		return;
	uint16_t len = 0;
	// Task_Parameter_Struct *TaskArray = e->taskArray;
	// Task_Control_Status TaskControlStatus;

/*Task_Parameter_Struct Task = { .mode = Task_VOR, .VOR.Counter = 5 };
TaskArray = &Task;*/
BEGIN_POS:
	// get all task need to implement
	while (1)
	{
		if (e->State_Bit.Init == 1)
		{
			e->State_Bit.flag = 0;
			e->Error_Bit.flag = 0;
			if (e == 0 || e->taskArray == 0)
			{
				e->Error_Bit.paraError = 1;
				continue;
			}
			break;
		}
		SaftExitDelay(100, 0);
	}

	e->State_Bit.IsRunning = 1;
	uint16_t task_size = e->taskCount;
	for (int i = 0; i < task_size; i++)
	{
		Task_Parameter_Struct* task = &e->taskArray[i];
		task_interval_handle(i);
		Message_Center_Send_prinft(
			"PAGE1", 0,
			"Begin:Running %d", i + 1);
		/*len = sprintf(msg_buffer, "Begin:Running %d", i + 1);
		Message_Center_Send("PAGE1", 0, msg_buffer, len);*/
		//HAL_Set_UI_Page1_Msg("Begin:Running %d", i + 1);
		switch (task->mode)
		{
		case Task_VOR:
			VorControlFunction(task, e);
			break;
		case Task_Continue:
			ContControlFunction(task, e);
			break;
		default:
			break;
		}
	}
	Message_Center_Send_prinft(
		"PAGE1", 0,
		"End");
	//len = sprintf(msg_buffer, "End");
	//Message_Center_Send("PAGE1", 0, msg_buffer, len);
	/*HAL_Set_UI_Page1_Msg("End");*/
	e->State_Bit.flag = 0;
	e->Error_Bit.flag = 0;
	goto BEGIN_POS;
}
