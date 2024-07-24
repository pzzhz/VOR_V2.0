#include "task_control.h"
#include "../other/system_function.h"
#include "../Task/task_info_struct.h"
#include "stdint.h"

#ifndef STM32F40_41xxx
#include "Windows.h"
#include <stdio.h>
#else
#include "FreeRTOS.h"
#include "task.h"
#endif	

#include "control_VOR.h"

Task_Parameter_Struct TaskArray[10];
typedef struct
{
	uint16_t currentTaskIndex;
	uint32_t remainingTime;
}Task_Control_Status;


void task_interval_handle()
{

}

void Task_control_handler()
{
	Task_Parameter_Struct* TaskArray=0;uint16_t count=0;
	Task_Control_Status TaskControlStatus;
	Mode_info_struct e = { 0 };
	uint16_t task_size = count;

	Task_Parameter_Struct Task = { .mode = Task_VOR, .VOR.Counter = 5 };
	TaskArray = &Task;
	task_size = 1;
	//get all task need to implement

	for (int i = 0;i < task_size;i++)
	{
		TaskControlStatus.currentTaskIndex = i;
		task_interval_handle();
		switch (TaskArray[i].mode)
		{
		case Task_VOR:
			VorControlFunction(&TaskArray[i], &e);
			break;
		case Task_Continue:
			break;
		default:
			break;
		}
	}

}

