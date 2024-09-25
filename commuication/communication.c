/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @Date: 2024-08-05 14:28:28
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @LastEditTime: 2024-09-23 17:19:07
 * @FilePath: \USERd:\workfile\��Ŀ3 vor\software\VOR_V2.0\commuication\communication.c
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "communication.h"
#include "../INTERFACE/UI/other/system_function.h"
#include "communication_API.h"

extern void jump_to_bootloader(void);

void communication_thread()
{
	HAL_communication_Init();
	while (1)
	{
//		if( HAL_USB_Isconnect())
//				jump_to_bootloader();
		ControlGetTick();
		HAL_BL_USART_Server();
		ControlDelay(1);
	}
}
uint32_t taskhand[2];
void Communication_Init()
{
	ControlThreadCreate(
		communication_thread,
		0,
		taskhand,
		"communication",
		256);
}