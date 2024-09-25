#include "delay.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
// ���ʹ��OS,����������ͷ�ļ�����ucosΪ��������.
#if SYSTEM_SUPPORT_OS
#include "includes.h" //֧��OSʱ��ʹ��
#endif
//////////////////////////////////////////////////////////////////////////////////
// ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
// ALIENTEK STM32F407������
// ʹ��SysTick����ͨ����ģʽ���ӳٽ��й���(֧��OS)
// ����delay_us,delay_ms
// ����ԭ��@ALIENTEK
// ������̳:www.openedv.com
// ��������:2014/5/2
// �汾��V1.3
// ��Ȩ���У�����ؾ���
// Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
// All rights reserved
//********************************************************************************
// �޸�˵��
// V1.1 20140803
// 1,delay_us,��Ӳ�������0�ж�,�����������0,��ֱ���˳�.
// 2,�޸�ucosii��,delay_ms����,����OSLockNesting���ж�,�ڽ����жϺ�,Ҳ����׼ȷ��ʱ.
// V1.2 20150411
// �޸�OS֧�ַ�ʽ,��֧������OS(������UCOSII��UCOSIII,����������OS������֧��)
// ���:delay_osrunning/delay_ostickspersec/delay_osintnesting�����궨��
// ���:delay_osschedlock/delay_osschedunlock/delay_ostimedly��������
// V1.3 20150521
// ����UCOSIII֧��ʱ��2��bug��
// delay_tickspersec��Ϊ��delay_ostickspersec
// delay_intnesting��Ϊ��delay_osintnesting
//////////////////////////////////////////////////////////////////////////////////

// static u8 fac_us = 0;  // us��ʱ������
// static u16 fac_ms = 0; // ms��ʱ������,��os��,����ÿ�����ĵ�ms��

#if SYSTEM_SUPPORT_OS // ���SYSTEM_SUPPORT_OS������,˵��Ҫ֧��OS��(������UCOS).
// ��delay_us/delay_ms��Ҫ֧��OS��ʱ����Ҫ������OS��صĺ궨��ͺ�����֧��
// ������3���궨��:
//     delay_osrunning:���ڱ�ʾOS��ǰ�Ƿ���������,�Ծ����Ƿ����ʹ����غ���
// delay_ostickspersec:���ڱ�ʾOS�趨��ʱ�ӽ���,delay_init�����������������ʼ��systick
//  delay_osintnesting:���ڱ�ʾOS�ж�Ƕ�׼���,��Ϊ�ж����治���Ե���,delay_msʹ�øò����������������
// Ȼ����3������:
//   delay_osschedlock:��������OS�������,��ֹ����
// delay_osschedunlock:���ڽ���OS�������,���¿�������
//     delay_ostimedly:����OS��ʱ,���������������.

// �����̽���UCOSII��UCOSIII��֧��,����OS,�����вο�����ֲ
// ֧��UCOSII
#ifdef OS_CRITICAL_METHOD					 // OS_CRITICAL_METHOD������,˵��Ҫ֧��UCOSII
#define delay_osrunning OSRunning			 // OS�Ƿ����б��,0,������;1,������
#define delay_ostickspersec OS_TICKS_PER_SEC // OSʱ�ӽ���,��ÿ����ȴ���
#define delay_osintnesting OSIntNesting		 // �ж�Ƕ�׼���,���ж�Ƕ�״���
#endif

// ֧��UCOSIII
#ifdef CPU_CFG_CRITICAL_METHOD				  // CPU_CFG_CRITICAL_METHOD������,˵��Ҫ֧��UCOSIII
#define delay_osrunning OSRunning			  // OS�Ƿ����б��,0,������;1,������
#define delay_ostickspersec OSCfg_TickRate_Hz // OSʱ�ӽ���,��ÿ����ȴ���
#define delay_osintnesting OSIntNestingCtr	  // �ж�Ƕ�׼���,���ж�Ƕ�״���
#endif

// us����ʱʱ,�ر��������(��ֹ���us���ӳ�)
void delay_osschedlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD // ʹ��UCOSIII
	OS_ERR err;
	OSSchedLock(&err); // UCOSIII�ķ�ʽ,��ֹ���ȣ���ֹ���us��ʱ
#else				   // ����UCOSII
	OSSchedLock(); // UCOSII�ķ�ʽ,��ֹ���ȣ���ֹ���us��ʱ
#endif
}

// us����ʱʱ,�ָ��������
void delay_osschedunlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD // ʹ��UCOSIII
	OS_ERR err;
	OSSchedUnlock(&err); // UCOSIII�ķ�ʽ,�ָ�����
#else					 // ����UCOSII
	OSSchedUnlock(); // UCOSII�ķ�ʽ,�ָ�����
#endif
}

// ����OS�Դ�����ʱ������ʱ
// ticks:��ʱ�Ľ�����
void delay_ostimedly(u32 ticks)
{
#ifdef CPU_CFG_CRITICAL_METHOD
	OS_ERR err;
	OSTimeDly(ticks, OS_OPT_TIME_PERIODIC, &err); // UCOSIII��ʱ��������ģʽ
#else
	OSTimeDly(ticks); // UCOSII��ʱ
#endif
}

// systick�жϷ�����,ʹ��OSʱ�õ�
void SysTick_Handler(void)
{
	if (delay_osrunning == 1) // OS��ʼ����,��ִ�������ĵ��ȴ���
	{
		OSIntEnter(); // �����ж�
		OSTimeTick(); // ����ucos��ʱ�ӷ������
		OSIntExit();  // ���������л����ж�
	}
}
#endif

// ��ʼ���ӳٺ���
// ��ʹ��OS��ʱ��,�˺������ʼ��OS��ʱ�ӽ���
// SYSTICK��ʱ�ӹ̶�ΪAHBʱ�ӵ�1/8
// SYSCLK:ϵͳʱ��Ƶ��
void delay_init(u8 SYSCLK)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;

	// #if SYSTEM_SUPPORT_OS 						//�����Ҫ֧��OS.
	// 	u32 reload;
	// #endif
	//  	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	// 	fac_us=SYSCLK/8;						//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
	// #if SYSTEM_SUPPORT_OS 						//�����Ҫ֧��OS.
	// 	reload=SYSCLK/8;						//ÿ���ӵļ������� ��λΪM
	// 	reload*=1000000/delay_ostickspersec;	//����delay_ostickspersec�趨���ʱ��
	// 											//reloadΪ24λ�Ĵ���,���ֵ:16777216,��168M��,Լ��0.7989s����
	// 	fac_ms=1000/delay_ostickspersec;		//����OS������ʱ�����ٵ�λ
	// 	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	// 	SysTick->LOAD=reload; 					//ÿ1/delay_ostickspersec���ж�һ��
	// 	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; 	//����SYSTICK
	// #else
	// 	fac_ms=(u16)fac_us*1000;				//��OS��,����ÿ��ms��Ҫ��systickʱ����
	// #endif
}

#if SYSTEM_SUPPORT_OS // �����Ҫ֧��OS.
// ��ʱnus
// nus:Ҫ��ʱ��us��.
// nus:0~204522252(���ֵ��2^32/fac_us@fac_us=21)
void delay_us(u32 nus)
{
	u32 ticks;
	u32 told, tnow, tcnt = 0;
	u32 reload = SysTick->LOAD; // LOAD��ֵ
	ticks = nus * fac_us;		// ��Ҫ�Ľ�����
	delay_osschedlock();		// ��ֹOS���ȣ���ֹ���us��ʱ
	told = SysTick->VAL;		// �ս���ʱ�ļ�����ֵ
	while (1)
	{
		tnow = SysTick->VAL;
		if (tnow != told)
		{
			if (tnow < told)
				tcnt += told - tnow; // ����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else
				tcnt += reload - tnow + told;
			told = tnow;
			if (tcnt >= ticks)
				break; // ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}
	};
	delay_osschedunlock(); // �ָ�OS����
}
// ��ʱnms
// nms:Ҫ��ʱ��ms��
// nms:0~65535
void delay_ms(u16 nms)
{
	if (delay_osrunning && delay_osintnesting == 0) // ���OS�Ѿ�������,���Ҳ������ж�����(�ж����治���������)
	{
		if (nms >= fac_ms) // ��ʱ��ʱ�����OS������ʱ������
		{
			delay_ostimedly(nms / fac_ms); // OS��ʱ
		}
		nms %= fac_ms; // OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ
	}
	delay_us((u32)(nms * 1000)); // ��ͨ��ʽ��ʱ
}
#else // ����ucosʱ
// ��ʱnus
// nusΪҪ��ʱ��us��.
// ע��:nus��ֵ,��Ҫ����798915us(���ֵ��2^24/fac_us@fac_us=21)
void delay_us(u32 nus)
{
	// const uint32_t SystemClock = 168000000;
	// if (nus > 1000)
	// 	nus = 1000;
	// TIM14->CR1 = 0;
	// TIM14->PSC = (168 / 2) - 1;
	// TIM14->ARR = (10000) - 1;
	// TIM14->CNT = 0;
	// TIM14->CR1 |= (TIM_CR1_CEN | TIM_CR1_OPM);
	// while (TIM14->CNT < nus)
	// {
	// }
	// return;
	uint32_t temp = 1 | TIM_CR1_OPM;
	if (nus > 1000)
		nus = 5000;


	TIM14->CR1 = 0;
	TIM14->PSC = (168/2) - 1;
	TIM14->ARR = (nus*10) - 1;
	TIM14->CNT = 0;
	__DSB();
	
	TIM14->CR1 |= temp;
	while (TIM14->CR1&TIM_CR1_CEN)
	{
	}
	return;

}
// ��ʱnms
// ע��nms�ķ�Χ
// SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
// nms<=0xffffff*8*1000/SYSCLK
// SYSCLK��λΪHz,nms��λΪms
// ��168M������,nms<=798ms
void delay_xms(u16 nms)
{
	// u32 temp;
	// SysTick->LOAD = (u32)nms * fac_ms;		  // ʱ�����(SysTick->LOADΪ24bit)
	// SysTick->VAL = 0x00;					  // ��ռ�����
	// SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // ��ʼ����
	// do
	// {
	// 	temp = SysTick->CTRL;
	// } while ((temp & 0x01) && !(temp & (1 << 16))); // �ȴ�ʱ�䵽��
	// SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // �رռ�����
	// SysTick->VAL = 0X00;					   // ��ռ�����
}
int xx=0;
// ��ʱnms
// nms:0~65535
void delay_ms(u16 nms)
{
	uint32_t temp = 1 | TIM_CR1_OPM;
	if (nms > 1000)
		nms = 5000;


	TIM14->CR1 = 0;
	TIM14->PSC = (16800/2) - 1;
	TIM14->ARR = (nms*10) - 1;
	TIM14->CNT = 0;
	__DSB();
	
	TIM14->CR1 |= temp;
	while (TIM14->CR1&TIM_CR1_CEN)
	{
	}
	return;

}

void jump_to_bootloader(void)
{
    // ���������ж�
    __disable_irq();

    // ���� Bootloader ��ַ
    uint32_t bootloader_address = 0x1FFF0000;

    // ����һ��ָ������ָ�룬ָ�� Bootloader �ĸ�λ����
    void (*bootloader_entry)(void) = (void (*)(void))(*((uint32_t *)(bootloader_address + 4)));

    // ��������ջָ��
	RCC->AHB1RSTR=RCC_AHB1RSTR_OTGHRST;
	RCC->AHB1RSTR=0;
    __set_MSP(*((uint32_t *)bootloader_address));

    // ��ת�� Bootloader
    bootloader_entry();
}

#endif
