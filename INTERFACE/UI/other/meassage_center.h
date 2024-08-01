#ifndef __Meassage_Center_H_
#define __Meassage_Center_H_

#include "stdint.h"
#include "string.h"
#ifdef __cplusplus
extern "C"
{
#endif //

#define COMPARE(a,b) strncmp(a,b,sizeof(a)-1)

	uint8_t Message_Center_Send(
		const char* name,
		uint16_t fun_id,
		void* source,
		char* msg,
		uint16_t msgLen);
	uint8_t Message_Center_Send_prinft(
		const char* name,
		uint16_t fun_id,
		void* source,
		const char* format,
		...);
	uint8_t Message_Center_Get(
		const char* name,
		uint16_t fun_id,
		void** source,
		char* msg,
		uint16_t msgLen);

	uint8_t Message_Center_Clean_Flag(
		const char* name,
		uint16_t fun_id);

	uint8_t Message_Center_Receive_Scanf(
		const char* name,
		uint16_t FilterId,
		void** source,
		const char* format,
		...);

	//0 match
	uint8_t Message_Center_Receive_Compare(
		const char* name,
		uint16_t FilterId,
		void** source,
		const char* str1);

	void Meassage_Center_Add(const char* name);
	void Message_Center_Init();
#ifdef __cplusplus
}
#endif //


#endif