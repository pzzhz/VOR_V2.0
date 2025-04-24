/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @Date: 2025-02-19 09:42:35
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @LastEditTime: 2025-04-05 07:49:12
 * @FilePath: \USERd:\workfile\项目3 vor\software\VOR_V2.0\HARDWARE\IO\GPIO.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "GPIO.h"
#include "stm32f4xx.h"


static void Timer_init(int ms)
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;

    TIM10->PSC = 14400;
    TIM10->ARR = 10 * 10;       //interval 10ms
    TIM10->CNT = 1;
    TIM10->CR1 |= TIM_CR1_CEN;
    TIM10->DIER |= TIM_DIER_UIE;
		TIM10->SR = 0;
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;  // 串口1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);                           // 根据指定的参数初始化VIC寄存器、
		// GPIOF->ODR |= GPIO_ODR_ODR_15;
}

void CAM_GPIO_INIT()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;

    GPIOG->MODER |= GPIO_MODER_MODER0_0;  // gpio g0 output init
    GPIOG->MODER |= GPIO_MODER_MODER1_0;  // alm
    GPIOF->MODER |= GPIO_MODER_MODER15_0; // gpio g0 output init

    GPIOG->ODR &= ~GPIO_ODR_ODR_0;  // REC
    GPIOG->ODR &= ~GPIO_ODR_ODR_1;  // WIFI
    GPIOF->ODR &= ~GPIO_ODR_ODR_15; // SYNC LED

		Timer_init(1500);
}

static int sec=0;
static int ReqSec=0;
void TIM1_UP_TIM10_IRQHandler()
{
  TIM10->SR = 0;
	
	if(sec>=ReqSec)
	{
		    GPIOG->ODR &= ~GPIO_ODR_ODR_0;
				GPIOF->ODR &= ~GPIO_ODR_ODR_15;
				GPIOG->ODR &= ~GPIO_ODR_ODR_1;
	}
	sec++;
}

void GPIO_CAM_REC_IO_Ctrl()
{
    GPIOG->ODR |= GPIO_ODR_ODR_1;
		sec=0;
		ReqSec=150;
    
}

void GPIO_CAM_SyncLED_IO_Ctrl()
{
    GPIOF->ODR |= GPIO_ODR_ODR_15;
		sec=0;
		ReqSec=5;
}

void GPIO_CAM_Wifi_IO_Ctrl()
{
    GPIO_SetBits(GPIOG,GPIO_Pin_0);
		sec=0;
    ReqSec=500;
}