/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @Date: 2024-08-05 14:28:28
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @LastEditTime: 2024-09-05 11:21:52
 * @FilePath: \USERd:\workfile\��Ŀ3 vor\software\VOR_V2.0\commuication\rk3588_uart.c
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "rk3588_uart.h"
#include "string.h"
#include "stdio.h"
#include <stdarg.h>
#include "../HARDWARE/USART/USART1.h"

#define buff_size 64
static uint16_t buffIndex;
static uint8_t buff[buff_size];
static uint8_t newlineFlag;
static uint8_t isreading;

void Rk3588_Printf(const char* strOutputString, ...)
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
	uint16_t len = vsnprintf(strBuffer, PUT_PUT_DEBUG_BUF_LEN, strOutputString, vlArgs);
	va_end(vlArgs);
	printf(strBuffer);
	OutputDebugStringA(strBuffer); // OutputDebugString    // OutputDebugStringW
	isbusy = 0;
#else
	extern uint8_t USART1_Send_Package(char* string, uint16_t size);
#define PUT_PUT_DEBUG_BUF_LEN 50
	static char strBuffer[PUT_PUT_DEBUG_BUF_LEN] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	// vsprintf(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);  //_vsnprintf_s  _vsnprintf
	uint16_t len = vsnprintf(strBuffer, PUT_PUT_DEBUG_BUF_LEN, strOutputString, vlArgs);
	va_end(vlArgs);
	uint8_t res = 0;
	while (res == 0)
	{
		res = USART1_Send_Package(strBuffer, len); // OutputDebugString    // OutputDebugStringW
		vTaskDelay(1);
	}

#endif
	isbusy = 0;
}

int Rk3588_ReadLine(const char* format, ...)
{
	if (newlineFlag != 1)
		return -1;
	isreading = 1;
	va_list args;
	va_start(args, format);
	int res = vsscanf(buff, format, args);
	buffIndex = 0;
	newlineFlag = 0;
	va_end(args);
	isreading = 0;
	return res;
}

int Rk3588_ReadLineArg(const char* format, va_list* args)
{
	if (newlineFlag != 1)
		return -1;
	isreading = 0;
	int res = vsscanf(buff, format, args);
	buffIndex = 0;
	newlineFlag = 0;
	isreading = 0;
	return res;
}

void Rk3588_SerialBuffer(uint8_t byte)
{
	if (isreading)
		return;
	if (buffIndex >= buff_size)
	{
		buffIndex = 0;
	}
	if (byte != '\r')
	{
		buff[buffIndex] = byte;
		buffIndex++;
	}
	if (byte == (uint8_t)'\n')
		newlineFlag = 1;
}

void Rk3588_Init(void)
{
#ifdef STM32F40_41xxx
	USART1_INIT(115200, Rk3588_SerialBuffer);
#else
	SerialPort_Init(Rk3588_SerialBuffer, L"COM5");
#endif
}
