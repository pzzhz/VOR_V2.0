#include "communication.h"
#include "../INTERFACE/UI/other/system_function.h"
#ifdef STM32F40_41xxx
#include "communication_API.h"

#endif

void communication_thread()
{
	while (1)
	{
		HAL_BL_USART_Server();
		ControlDelay(50);
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
		512);
}