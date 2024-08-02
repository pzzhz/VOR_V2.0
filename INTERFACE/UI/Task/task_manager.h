#ifndef  __task_manager_H_
#define __task_manager_H_
#ifdef __cplusplus
extern "C"
{
#endif //
#include "task_info_struct.h"

	uint8_t Task_manager_Req_add(uint32_t handleID,
		uint16_t fouce_index,
		Task_Parameter_Struct info);

	uint8_t Task_manager_Req_Save(uint32_t handleID,
		uint16_t fouce_index,
		Task_Parameter_Struct info);

	uint8_t Task_manager_Req_saveCreate(uint32_t handleID,
		uint16_t fouce_index,
		Task_Parameter_Struct info);

	uint8_t Task_manager_Req_Del(uint32_t handleID,
		uint16_t fouce_index);

	uint8_t Task_manager_Req_Move(uint32_t handleID,
		uint16_t swap1, uint16_t swap2);

	uint8_t Task_manager_Begin_Req(uint32_t* handleID);

	uint8_t Task_manager_End_release();

	uint8_t Task_manager_ADD_GetResult(uint32_t handleID, void* pt);

	void Task_mangager_Init();
#ifdef __cplusplus
}
#endif // 

#endif // 