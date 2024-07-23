#ifndef  __control_h_
#define __control_h_
#ifdef __cplusplus
extern "C"
{
#endif //
#include "stdint.h"
#include "../Task/task_info_struct.h"
#ifndef STM32F40_41xxx
#include "Windows.h"
#include <stdio.h>
#endif // use_windows

#define control_cb_array_size 10
#define CmdCheck(in,ref) strncmp(in, ref, sizeof(ref)-1)

	typedef enum
	{
		ready,
		taskruning,
		runing,
		end,
	}task_runing;
	typedef enum
	{
		CMD_add,
		CMD_Move,
		CMD_Del,
		CMD_Set,
	}CMD_Enum;

	typedef struct {
		uint8_t ExitFlag;
		uint8_t FinishFlag;
#ifndef STM32F40_41xxx
		HANDLE hThread;
#endif // use_windows
		struct
		{
			uint32_t sec;
			task_runing state;
		}UI_para;
		uint8_t* message;
	}Mode_info_struct;
	void controlInit();
	void SaftExitDelay(uint32_t ms, Mode_info_struct* e);
	uint32_t  ControlGetTick();
	//0 代表无返回信息
	typedef uint8_t(*UI_function)(uint8_t* cmd, uint8_t* meassage, Mode_info_struct* e);
	typedef struct
	{
		UI_function cb;
		uint8_t message[50];
	}UI_Function_struct;
	uint8_t Control_Set_cb(UI_Function_struct* function_array,
		uint16_t index,
		UI_function cb);
	uint8_t Control_Remove_cb(UI_Function_struct* function_array,
		uint16_t index,
		UI_function cb);


	extern UI_Function_struct control_cb_array[control_cb_array_size];
#ifdef __cplusplus
}
#endif // 

#endif // ! UI_Paremeter_Set_H_
