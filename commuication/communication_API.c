#include "communication_API.h"

#ifdef STM32F40_41xxx
#define USE_USB 0
#define USE_BL 1
#else
#define USE_USB 0
#define USE_BL 1
#include "../serial/serialPort.h"
#endif

#if USE_USB
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"
#include "usbd_cdc_core.h"

USB_OTG_CORE_HANDLE USB_OTG_dev;
extern USBD_Usr_cb_TypeDef USR_cb;
#endif

#if USE_BL

#include "BLUETOOTH/BlueTooth.h"
#include "../HARDWARE/USART/USART3.h"
#include "../HARDWARE/USART/USART1.h"


#endif

#define USE_MPU 1
#if USE_MPU
#include "mpu/jy60.h"
#endif

// 命令窗口 需要线程

// log print

// oriange pie

// ble 要开线程通信

void HAL_communication_Bl_Init(void)
{
	// 串口 init
}

// yj-60 init
void HAL_communication_Mpu_Init(void)
{
#if USE_MPU
	JY60_Init();
#endif
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
	USART1_INIT(9600,
		usart_protocol_InterruptHandle);
#else
	SerialPort_Init(usart_protocol_InterruptHandle);
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
}

void HAL_communication_Init(void)
{
	HAL_communication_Mpu_Init();
	HAL_communication_Bl_Init();
	HAL_BL_USART_INIT();
}
