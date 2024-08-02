/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-24 14:38:21
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-08-02 11:16:25
 * @FilePath: \USER\hard_test\test_main.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "stm32f4xx.h"
#include "../SLAVE/Slave1_IO.h"
#include "../SLAVE/Slave1_Timer.h"
#include "string.h"
#include "../USART/USART5.h"
#include "../USART/USART3.h"
#include "math.h"
#include "../SYSTEM/delay/delay.h"
#include "stdio.h"

#include "../FREERTOS/source/include/FreeRTOS.h"
#include "../FREERTOS/source/include/task.h"

extern void slave_test(void);
extern uint8_t CONT_Machine_Init(float vel, uint32_t MillSec);
double ss = 9.95f;
char *srs = "%.2f %d ";
void main()
{
    extern uint8_t HAL_USB_INIT();
    extern void Communication_Init();
    extern void HAL_communication_Init(void);
    delay_init(168);
    Slave1_IO_Init();
    Slave1_Step_Generator_Init(50000, 10);
    Slave1_En_IO(1);
    delay_init(168);
    CONT_Machine_Init(30, 10 * 1000);
    //    HAL_USB_INIT();
    //    USART1_INIT(9600, 0);
    Communication_Init();
    HAL_communication_Init();
    vTaskStartScheduler();
    // //    USART5_INIT(9600, jy60_decode);
    // while (1)
    // {
    //     delay_ms(500);

    //     /* code */
    // }
}