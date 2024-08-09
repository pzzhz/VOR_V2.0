/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-25 14:38:15
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-07-29 14:53:03
 * @FilePath: \USERd:\workfile\��Ŀ3 vor\software\VOR_V2.0\INTERFACE\UI\control\control_Hardware_API.h
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __control_Hardware_API_H_
#define __control_Hardware_API_H_
#include "stdint.h"
#include "../Task/task_stroage.h"
#include "../other/meassage_center.h"
#ifdef __cplusplus
extern "C"
{
#endif //
#ifndef STM32F40_41xxx
#define MYPRINTF(...) printf(__VA_ARGS__)
#else
#define MYPRINTF(...)
#endif
	typedef enum
	{
		StartCmdNone,
		StartCmdBegin,
		StartCmdStop
	} Start_Cmd_Type;

	uint8_t HAL_Incline_Init(float angle, uint32_t time);
	uint8_t HAL_Incline_Get_State(float *angle);

	uint8_t HAL_CAM_Init(void);
	uint8_t HAL_CAM_REC_Set(uint8_t flag);
	uint8_t HAL_CAM_SET_Set(void);

	uint8_t HAL_Control_Get_Start_Cmd(Start_Cmd_Type *cmd);
	uint8_t HAL_Set_UI_Page1_Msg(const char *format, ...);
	uint16_t HAL_Task_GetArray(Task_Parameter_Struct *taskarray, uint16_t arraySize);

	uint8_t HAL_API_INIT(void);

#ifdef __cplusplus
}
#endif //

#endif //