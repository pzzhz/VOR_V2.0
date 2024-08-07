/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-08-05 11:22:48
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-08-05 11:27:53
 * @FilePath: \USERd:\workfile\��Ŀ3 vor\software\VOR_V2.0\HARDWARE\IR_Sense\IR.c
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "IR.h"
#include "stm32f4xx.h"
// 0:none 1:reach
uint8_t IR_IO_Get(void)
{
    uint32_t res = GPIOE->IDR;
    res &= GPIO_Pin_6;
    return (res == 0) ? 1 : 0;
}

void IR_IO_Init(void)
{

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    GPIOE->MODER &= ~GPIO_MODER_MODER6;

}
