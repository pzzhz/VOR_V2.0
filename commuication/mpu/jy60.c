/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-30 11:37:02
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @LastEditTime: 2024-09-05 18:22:21
 * @FilePath: \USERd:\workfile\项目3 vor\software\VOR_V2.0\commuication\mpu\jy60.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "jy60.h"
#include "math.h"
#include "string.h"
#include "stdio.h"
// #include "..\HARDWARE\USART\USART5.h"

static uint8_t isupdata;

struct
{
    int16_t roll;
    int16_t pitch;
    int16_t yaw;
} jy60_orginal;

typedef struct
{
    float roll;
    float pitch;
    float yaw;
    /* data */
} jy60_data;
struct
{
    jy60_data data;
    float inc;
    uint8_t (*handle)();
} jy60;

static jy60_data Calibrate;
extern uint8_t USART3_PRINTF(char *string, uint16_t size);

void jy60_Calculate()
{
    static jy60_data stroage[10];
    jy60_data avarage = {0};
    static uint16_t index;
    isupdata = 1;
    jy60.data.roll = jy60_orginal.roll / 32768.0f * 180 - Calibrate.roll;
    jy60.data.pitch = jy60_orginal.pitch / 32768.0f * 180 - Calibrate.pitch;
    jy60.data.yaw = jy60_orginal.yaw / 32768.0f * 180 - Calibrate.yaw;
    jy60.inc = sqrt((jy60.data.roll) * (jy60.data.roll) + (jy60.data.pitch) * (jy60.data.pitch));
    char str[50];
    uint16_t len = sprintf(str, "\r\n %d.%02d", (int)jy60.inc, (int)(jy60.inc * 100.0f) % 100);
    //USART3_PRINTF(str, len);
    if (index == 10)
        return;

    jy60.data.roll = jy60_orginal.roll / 32768.0f * 180;
    jy60.data.pitch = jy60_orginal.pitch / 32768.0f * 180;
    jy60.data.yaw = jy60_orginal.yaw / 32768.0f * 180;
    isupdata = 0;
    stroage[index] = jy60.data;
    index++;

    // index = (index < sizeof(stroage)) ? index : 0;
    if (index == 10)
    {
        for (uint16_t i = 0; i < 9; i++)
        {
            avarage.roll += stroage[i].roll;
            avarage.pitch += stroage[i].pitch;
            avarage.yaw += stroage[i].yaw;
        }
        avarage.roll /= 10;
        avarage.pitch /= 10;
        avarage.yaw /= 10;
        Calibrate = avarage;
    }
}

void jy60_decode(uint8_t dr)
{
    const uint8_t match_head[] = {0x55, 0x53};
    static uint8_t bytes[11];
    static uint16_t current_Index;
    switch (current_Index)
    {
    case 0:
    case 1:
        if (dr == match_head[current_Index])
        {
            bytes[current_Index] = dr;
            current_Index++;
        }
        break;
    case 10:
        isupdata = 1;
        uint8_t result = 0;
		 bytes[current_Index] = dr;
        for (int i = 0; i < 10; i++)
        {
            result +=bytes[i];
        }
				 current_Index = 0;
        if (result != dr)
				{
					isupdata=0;
					return;
				}

        memcpy(&jy60_orginal, &bytes[2], sizeof(jy60_orginal));
        jy60_Calculate();
       
        break;
    default:
        if (current_Index < 11)
        {
            bytes[current_Index] = dr;
            current_Index++;
        }
        break;
    }
}

uint8_t JY60_Get_Inc(float *angle)
{
    if (isupdata == 0)
        return 0;
    if (angle != 0)
        *angle = jy60.inc;
    isupdata = 0;
    return 1;
}

float JY60_Get_IncAsync()
{
    return jy60.inc;
}

void JY60_Init(void)
{
#ifdef STM32F40_41xxx
    USART5_INIT(9600, jy60_decode);

#endif //  STM32F40_41xxx
}