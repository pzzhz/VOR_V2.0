/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-08-05 09:13:41
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-08-05 11:07:55
 * @FilePath: \USERd:\workfile\��Ŀ3 vor\software\VOR_V2.0\implement\Slave_Inc_Ctrl.h
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __Slave_OVAR_Ctrl_H_
#define __Slave_OVAR_Ctrl_H_
#include "stdint.h"

#ifdef __cplusplus
extern "C"
{
#endif

    uint8_t OVAR_Machine_Init(float vel, uint32_t MillSec);
    uint8_t OVAR_Machine_Stop(void);
    uint8_t OVAR_Machine_Get_Count(uint32_t *angleReq, uint32_t *CurrentAngle);

#ifdef __cplusplus
}
#endif

#endif