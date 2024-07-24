#include "control.h"
#include "task_control.h"
#include "../other/system_function.h"
#include "stdint.h"

#ifndef STM32F40_41xxx
#include "Windows.h"
#include <stdio.h>
#else
#include "FreeRTOS.h"
#include "task.h"
#endif // use_windows

#define use_windows

void thread_create(void* function, Mode_info_struct* e);

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
	int flag = 0;
	extern void VorControlFunction(Mode_info_struct * e);
	Mode_info_struct e = { 0 };
	Task_Parameter_Struct info;

	char message[50], cmd[50] = { "" };
	memset(control_cb_array, 0, sizeof(control_cb_array));
	while (1)
	{
		// UI 信息发送
		// UI 按键监听
		// UI Table 按键监听和处理
		// 串口 监听
		// 串口 反馈
		// 紧急停机
		//启动
		if (flag)
		{
			thread_create(Task_control_handler, &e);
			flag = 0;
		}
		control_run_cb_array(control_cb_array, control_cb_array_size, cmd, message, &info); // 给ui发消息？
		ControlDelay(10);
		// e.ExitFlag = 1;
	}
}

void thread_create(void* function, Mode_info_struct* e)
{
#ifndef STM32F40_41xxx
	HANDLE hThread;
	DWORD dwThreadId;
	hThread = CreateThread(NULL,         // 默认安全属性
		0,            // 默认堆栈大小
		function,     // 线程函数
		e,            // 传递给线程函数的参数
		0,            // 默认创建标志
		&dwThreadId); // 线程ID)
#else
	static TaskHandle_t control_thread;
	xTaskCreate((TaskFunction_t)function,
		(const char*)"Outside_motor",
		(uint16_t)512,
		(void*)e,
		(UBaseType_t)2,
		(TaskHandle_t*)&control_thread);
#endif
}

void controlInit()
{
	thread_create(controlfunction, 0);
}
