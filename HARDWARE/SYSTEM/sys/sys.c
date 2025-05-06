#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
// 本程序只供学习使用，未经作者许可，不得用于其它任何用途
// ALIENTEK STM32F407开发板
// 系统时钟初始化
// 包括时钟设置/中断管理/GPIO设置等
// 正点原子@ALIENTEK
// 技术论坛:www.openedv.com
// 创建日期:2014/5/2
// 版本：V1.0
// 版权所有，盗版必究。
// Copyright(C) 广州市星翼电子科技有限公司 2014-2024
// All rights reserved
//********************************************************************************
// 修改说明
// 无
//////////////////////////////////////////////////////////////////////////////////

// THUMB指令不支持汇编内联
// 采用如下方法实现执行汇编指令WFI
__asm void WFI_SET(void)
{
	WFI;
}
// 关闭所有中断(但是不包括fault和NMI中断)
__asm void INTX_DISABLE(void)
{
	CPSID I
		BX LR
}
// 开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE I
		BX LR
}
// 设置栈顶地址
// addr:栈顶地址
__asm void MSR_MSP(u32 addr)
{
	MSR MSP, r0 // set Main Stack value
				 BX r14
}

__asm int readsps(void)
{
	MOV R0, sp // 所嵌入的汇编代码
				bx lr
}

uint8_t *dfuFlag=(uint8_t *)0x2001FFF5;
typedef void (*bootapp)(void); // 定义一个函数类型的参数.
void JumpBootLoader()
{

	
	u32 Bootaddr = 0x1FFF0000;
	bootapp jumpBootLoader;
	jumpBootLoader = (bootapp) * (vu32 *)(Bootaddr + 4); // 系统存储器第二个字为程序开始地址
	MSR_MSP(*(vu32 *)Bootaddr);							 // 初始化堆栈指针
	jumpBootLoader();
}

void RequestEnterDFU(void)
{
	*dfuFlag=0xAA;
	SCB->AIRCR = (0X5FA << 16) + 4; //软件复位
}

void RequestReset(void)
{
	SCB->AIRCR = (0X5FA << 16) + 4; //软件复位
}

void DFU_PowerUp_Detection()
{
	if (*dfuFlag ==0xAA)    //判断上次复位是否为软件复位
    {
			*dfuFlag=0;
        RCC->CSR = 0;              //清除标志位
        JumpBootLoader();                    //进入bootloader
    }
    RCC->CSR=RCC_CSR_RMVF; 
		*dfuFlag=0;
}

