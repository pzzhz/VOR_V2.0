/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-23 16:30:52
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-07-31 14:55:37
 * @FilePath: \USERd:\workfile\项目3 vor\software\VOR_V2.0\INTERFACE\UI\other\system_function.c
 * @Description: 这是默�?��?�置,请�?�置`customMade`, 打开koroFileHeader查看配置 进�?��?�置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "system_function.h"
#include "string.h"
#ifndef STM32F40_41xxx
#include "Windows.h"
#include <stdio.h>
#else
#include "../FREERTOS/source/include/FreeRTOS.h"
#include "../FREERTOS/source/include/task.h"
#include "FreeRTOS.h"
#include "stdarg.h"
#include "task.h"
#endif // use_windows

void ControlDelay(uint32_t ms)
{
#ifndef STM32F40_41xxx
	Sleep(ms);
#else
	vTaskDelay(ms);
#endif // use_windows
}

void SaftExitDelay(uint32_t ms, uint8_t e)
{
	if (e)
	{
#ifndef STM32F40_41xxx
		ExitThread(0);
#else
		vTaskDelete(0);
#endif // use_windows
	}
#ifndef STM32F40_41xxx
	Sleep(ms);
#else
	vTaskDelay(ms);
#endif // use_windows
}

uint32_t ControlGetTick()
{
#ifndef STM32F40_41xxx
	static DWORD BeginTick = 0;
	if (BeginTick == 0)
		BeginTick = GetTickCount();
	uint32_t tick = GetTickCount() - BeginTick;
	return tick;

#else
	uint32_t tick = xTaskGetTickCount() / 20;
	return tick;
#endif
}

void OutputDebugPrintf(const char* strOutputString, ...)
{
	static uint8_t isbusy = 0;
	uint8_t* pt = &isbusy;
	while (*pt)
	{
	}
	isbusy = 1;
#ifndef STM32F40_41xxx
#define PUT_PUT_DEBUG_BUF_LEN 1024
	static char strBuffer[PUT_PUT_DEBUG_BUF_LEN] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	// vsprintf(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);  //_vsnprintf_s  _vsnprintf
	uint16_t len = vsprintf(strBuffer, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugStringA(strBuffer); // OutputDebugString    // OutputDebugStringW
	isbusy = 0;
#else
	extern uint8_t USART1_Send_Package(char* string, uint16_t size);
#define PUT_PUT_DEBUG_BUF_LEN 50
	static char strBuffer[PUT_PUT_DEBUG_BUF_LEN] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	// vsprintf(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);  //_vsnprintf_s  _vsnprintf
	uint16_t len = vsprintf(strBuffer, strOutputString, vlArgs);
	va_end(vlArgs);
	uint8_t res = 0;
	while (res == 0)
	{
		res = USART1_Send_Package(strBuffer, len); // OutputDebugString    // OutputDebugStringW
		vTaskDelay(1);
	}

#endif
}

long ControlThreadCreate(void* function,     // set thread code
	void* e,            // set parameter
	void* taskhandle_t, // thread instant
	const char* name,   // thread name
	uint16_t sizeofstack)
{
#ifndef STM32F40_41xxx
	HANDLE hThread;
	DWORD dwThreadId;
	hThread = CreateThread(NULL,         // 默�?�安全属�?
		sizeofstack,  // 默�?�堆栈大�?
		function,     // 线程函数
		e,            // 传递给线程函数的参�?
		0,            // 默�?�创建标�?
		&dwThreadId); // 线程ID)

#else
	//	static TaskHandle_t control_thread;
	volatile BaseType_t res =
		xTaskCreate((TaskFunction_t)function,
			(const char*)name,
			(uint16_t)sizeofstack,
			(void*)e,
			(UBaseType_t)2,
			(TaskHandle_t*)&taskhandle_t);
	return res;
#endif
	return 0;
}
