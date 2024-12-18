#include "communication_API.h"
#include "stdarg.h"


#ifdef STM32F40_41xxx
#define USE_USB 1
#define USE_BL 1
#else
#define USE_USB 0
#define USE_BL 1
#include "../serial/serialPort.h"
#endif

#ifndef STM32F40_41xxx
#include <windows.h>
#endif

#if USE_USB
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"
#include "usbd_cdc_core.h"

extern USB_OTG_CORE_HANDLE USB_OTG_dev;
extern USBD_Usr_cb_TypeDef USR_cb;
#endif

#if USE_BL

#include "BLUETOOTH/BlueTooth.h"
#include "../HARDWARE/USART/USART3.h"
#include "rk3588_uart.h"
#endif

#define USE_MPU 1
#if USE_MPU
#include "mpu/jy60.h"
#endif


// ����� ��Ҫ�߳�

// log print

// oriange pie

// ble Ҫ���߳�ͨ��

void HAL_communication_Bl_Init(void)
{
	// ���� init
}

// yj-60 init
void HAL_communication_Mpu_Init(void)
{
#if USE_MPU
	JY60_Init();
#endif
}

void Hal_Rk3588_Printf(const char* strOutputString, ...)
{
	//     va_arg
		//Rk3588_Printf(strOutputString,...);
}

int Hal_Rk3588_ReadLine(const char* format, ...)
{
	va_list arg;
	va_start(arg, format);
	int res = Rk3588_ReadLineArg(format, &arg);
	va_end(arg);
	return res;
	//    Rk3588_Printf(strOutputString,...);
}

int Hal_Rk3588_Readarray(char *bytes)
{
	return Rk3588_ReadLinearray(bytes);
	//    Rk3588_Printf(strOutputString,...);
}

int Hal_Rk3588_L_Readarray(char* bytes)
{
	return Rk3588_L_ReadLinearray(bytes);
	//    Rk3588_Printf(strOutputString,...);
}

int Hal_Rk3588_L_ReadLine(const char* format, ...)
{
	va_list arg;
	va_start(arg, format);
	int res = Rk3588_L_ReadLineArg(format, &arg);
	va_end(arg);
	return res;
	//    Rk3588_Printf(strOutputString,...);
}

uint8_t HAL_MPU_Get_Angle(float* angle)
{
#if USE_MPU
	return JY60_Get_Inc(angle);
#else
	return 0;
#endif
}

uint8_t HAL_BL_USART_INIT(void)
{
	extern void usart_protocol_InterruptHandle(uint8_t dr);
#if USE_BL
#ifdef STM32F40_41xxx
	USART3_INIT(9600,
		0);
	usart_protocol_init(USART3_Send_Package);
#else
	//SerialPort_Init(usart_protocol_InterruptHandle, L"COM1");
	usart_protocol_init(SerialPort_SendBuf);
#endif // STM32F40_41xxx

#endif
	return 1;
}

uint8_t HAL_BL_USART_Server(void)
{
#if USE_BL
	usart_protocol_decoding(0);
#endif
	return 1;
}

uint8_t HAL_USB_INIT()
{
#if USE_USB
	USBD_Init(&USB_OTG_dev,
		USB_OTG_FS_CORE_ID,
		&USR_desc,
		&USBD_CDC_cb,
		&USR_cb);
#endif
	return 1;
}

int HAL_USB_Isconnect()
{
#if USE_USB
extern	vu8 bDeviceState;	
#endif
	//return bDeviceState;
}

uint8_t HAL_USB_TX(uint8_t* str, uint16_t len)
{
#ifdef STM32F40_41xxx

	extern void usbd_cdc_senddata(void* pdev, uint8_t * str, uint16_t len);
	usbd_cdc_senddata(&USB_OTG_dev, str, len);
#endif // STM32F40_41xxx
	return 0;
}

void HAL_communication_Init(void)
{
	Rk3588_Init();
	HAL_communication_Mpu_Init();
	HAL_communication_Bl_Init();
	HAL_BL_USART_INIT();
//   HAL_USB_INIT();
}
