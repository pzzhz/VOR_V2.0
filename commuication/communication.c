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