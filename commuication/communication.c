/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @Date: 2024-08-05 14:28:28
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @LastEditTime: 2024-09-05 17:49:24
 * @FilePath: \USERd:\workfile\项目3 vor\software\VOR_V2.0\commuication\communication.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "communication.h"
#include "../INTERFACE/UI/other/system_function.h"
#include "communication_API.h"

void communication_thread()
{
	HAL_communication_Init();
	while (1)
	{
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