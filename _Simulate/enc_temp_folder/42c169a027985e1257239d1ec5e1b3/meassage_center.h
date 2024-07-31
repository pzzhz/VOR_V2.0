#ifndef __Meassage_Center_H_
#define __Meassage_Center_H_

#include "stdint.h"
#ifdef __cplusplus
extern "C"
{
#endif //
	uint8_t Message_Center_Send(
		const char* name,
		uint16_t fun_id,
		char* msg,
		uint16_t msgLen);
	uint8_t Message_Center_Get(
		const char* name,
		uint16_t* fun_id,
		char* msg,
		uint16_t msgLen);
	void Meassage_Center_Add(const char* name);
	void Message_Center_Init();
#ifdef __cplusplus
}
#endif //


#endif