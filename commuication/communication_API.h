/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-30 11:26:20
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-07-31 11:41:10
 * @FilePath: \USERd:\workfile\��Ŀ3 vor\software\VOR_V2.0\commuication\communication_API.h
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AEuint8_t HAL_BL_USART_INIT(void)
 */
#ifndef  __communication_API_H
#define __communication_API_H

#ifdef __cplusplus
extern "C"
{
#endif //
#include "stdint.h"

	uint8_t HAL_BL_USART_Server(void);
	void HAL_communication_Init(void);
	int Hal_Rk3588_ReadLine(const char* format, ...);

#ifdef __cplusplus
}
#endif //

#endif // ! __communication_API_H