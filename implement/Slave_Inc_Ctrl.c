/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-24 14:44:19
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-08-05 10:16:57
 * @FilePath: \USERd:\workfile\项目3 vor\software\VOR_V2.0\implement\Slave_Vor_Ctrl.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "stm32f4xx.h"
#include "Slave_Inc_Ctrl.h"
#include "math.h"
#include "delay.h"
#include "../HARDWARE/SLAVE/Slave1_IO.h"
#include "../HARDWARE/SLAVE/Slave1_Timer.h"
#include "../HARDWARE/INC/INC_CTRl.h"

typedef struct
{
    enum
    {
        running,
        end,
        retract,
        error
    } state;
    uint32_t Tick;
    uint32_t MaxCount;
    float angleReq;
} Inc_Machine_parameter;
Inc_Machine_parameter inc_para;

extern uint8_t JY60_Get_Inc(float *angle);

uint8_t Inc_ctrl(void)
{
    static uint8_t Angle_nonreqTime;
    static float angle = 0;
    if (JY60_Get_Inc(&angle) == 0)
    {
        //        Angle_nonreqTime++;
        //        if (Angle_nonreqTime == 255)
        //        {
        //            inc_para.state = error;
        //            return 1;
        //        }
    }
    else
        Angle_nonreqTime = 0;
    if (inc_para.state == running)
    {
        int16_t bias = ((inc_para.angleReq - angle) * 5.0f);
        if (bias > 0)
        {
            INC_IO_Set(1);
        }
        else if (bias < 0)
        {
            INC_IO_Set(2);
        }
        else
        {
            INC_IO_Set(0);
            if (inc_para.angleReq < 0.5)
            {
                inc_para.state = retract;
            }
            else
            {
                inc_para.state = end;
            }
        }
    }
    if (inc_para.state == retract)
    {
        inc_para.Tick++;
        INC_IO_Set(2);
        if (inc_para.Tick > 3000)
            inc_para.state = end;
    }
    if (inc_para.state == end)
    {
        return 1;
    }
    return 0;
}

uint8_t INC_handler(void)
{
    uint8_t res = 0;
    res = Inc_ctrl();
    if (res)
    {
        return 1; // end turn off tim4
    }
    return 0;
}

uint8_t INC_Machine_Init(float angle, uint32_t Maxcount)
{
#ifndef HARDWARE_TEST
#else
    INC_IO_INIT();
    Slave1_Set_Machine_Cb(INC_handler);
    inc_para.state = running;
    inc_para.angleReq = angle;
#endif
    return 1;
}

uint8_t INC_Machine_Stop(void)
{
    inc_para.state = end;
    return 1;
}

uint8_t INC_Machine_Get_Count(uint32_t *angleReq, uint32_t *CurrentAngle)
{
    if (inc_para.state == end)
        return 0;
    if (angleReq != 0)
        *angleReq = inc_para.angleReq;
    if (CurrentAngle != 0)
        *CurrentAngle = 0;
    return 1;
}
