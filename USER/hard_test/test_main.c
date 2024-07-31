/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-24 14:38:21
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-07-24 18:07:23
 * @FilePath: \USER\hard_test\test_main.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "stm32f4xx.h"
#include "../SLAVE/Slave1_IO.h"
#include "../SLAVE/Slave1_Timer.h"
extern void slave_test(void);
/***
 * 定时器 999 83 =1000hz
 * */
void TIM4_Motor_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); /// 使能TIM4时钟

    TIM_TimeBaseInitStructure.TIM_Period = arr;                     // 自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;                  // 定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure); // 初始化TIM4

    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); // 允许定时器3更新中断
    TIM_Cmd(TIM4, ENABLE);                     // 使能定时器3

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;              // 定时器3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; // 抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;        // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void main()
{
    Slave1_IO_Init();
    Slave1_Step_Generator_Init(50000, 10);
    Slave1_En_IO(1);
		delay_init(168);
        TIM4_Motor_Int_Init(999,83);
    while (1)
    {
       
        /* code */
    }
    
}