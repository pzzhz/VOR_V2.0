#include "control.h"
#include "control_Hardware_API.h"
#include "task_control.h"
#include "../other/system_function.h"
#include "stdint.h"
#include "../other/meassage_center.h"

#ifndef STM32F40_41xxx
#include "Windows.h"
#include <stdio.h>
#else
#include "FreeRTOS.h"
#include "task.h"
#endif // use_windows

#define use_windows
#ifdef STM32F40_41xxx
#else
typedef uint32_t TaskHandle_t;
#endif
void thread_create(void* function, Task_control_info* e, TaskHandle_t* control_thread, uint16_t sizeofstack);
TaskHandle_t task_ctrl_thread, contrl_thread;

UI_Function_struct control_cb_array[control_cb_array_size];
uint8_t Control_Set_cb(UI_Function_struct* function_array,
	uint16_t index,
	UI_function cb)
{
	for (int i = 0; i < index; i++)
	{
		if (function_array[i].cb == 0)
		{
			function_array[i].cb = cb;
			return 1;
		}
	}
	return 0;
}

uint8_t Control_Remove_cb(UI_Function_struct* function_array,
	uint16_t index,
	UI_function cb)
{
	for (int i = 0; i < index; i++)
	{
		if (function_array[i].cb == cb)
		{
			function_array[i].cb = 0;
			return 1;
		}
	}
	return 0;
}

void control_run_cb_array(UI_Function_struct* function_array,
	uint16_t index,
	char cmd[50],
	char message[50],
	Task_Parameter_Struct* e)
{
	for (int i = 0; i < index; i++)
	{
		if (function_array[i].cb != 0)
		{
			uint8_t res = function_array[i].cb(message, cmd, 0);
			/*if (function_array[i].cb(message, cmd, e))*/
			/*return;*/
		}
	}
}

void controlfunction()
{
	static Task_Parameter_Struct taskarray[15];
	int flag = 0;
	Task_control_info e = { 0 };
	Task_Parameter_Struct info;
	Start_Cmd_Type Startflag = StartCmdNone;
	uint8_t nowState = 0;
	char message[50], cmd[50] = { "" };
	memset(control_cb_array, 0, sizeof(control_cb_array));
	thread_create(Task_control_handler, &e, &task_ctrl_thread, 1000);
	Meassage_Center_Add("PAGE1");
	HAL_API_INIT();
	while (1)
	{
		// UI 信息发送
		// UI 按键监听
		// 串口 监听
		// 串口 反馈
		// 紧急停机
		// 启动
		// UI Table 按键监听和处理
		HAL_Control_Get_Start_Cmd(&Startflag);
		if (Startflag == StartCmdBegin)
		{
			Task_control_Begin(&e);
		}
		control_run_cb_array(control_cb_array, control_cb_array_size, cmd, message, &info); // 给ui发消息？
		ControlDelay(10);
		// e.ExitFlag = 1;
	}
}

void thread_create(void* function, Task_control_info* e, TaskHandle_t* control_thread, uint16_t sizeofstack)
{
#ifndef STM32F40_41xxx
	HANDLE hThread;
	DWORD dwThreadId;
	hThread = CreateThread(NULL,		 // 默认安全属性
		0,			 // 默认堆栈大小
		function,	 // 线程函数
		e,			 // 传递给线程函数的参数
		0,			 // 默认创建标志
		&dwThreadId); // 线程ID)
#else
	//	static TaskHandle_t control_thread;
	volatile BaseType_t res =
		xTaskCreate((TaskFunction_t)function,
			(const char*)"Outside_motor",
			(uint16_t)sizeofstack,
			(void*)e,
			(UBaseType_t)2,
			(TaskHandle_t*)&control_thread);
#endif
}

void controlInit()
{
	thread_create(controlfunction, 0, &contrl_thread, 750);
}
