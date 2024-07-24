/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-22 16:00:07
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-07-23 16:27:30
 * @FilePath: \USERd:\workfile\项目3 vor\software\VOR_V2.0\INTERFACE\UI\control\control_VOR.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "control_VOR.h"
#include "control.h"
#include "stdint.h"
#define use_windows
#ifndef STM32F40_41xxx
#include "Windows.h"
#include <stdio.h>
#endif // use_windows
 //无UI控制
 //返回message



void VorControlFunction(Task_Parameter_Struct* task, Mode_info_struct* e)
{
	const uint32_t begin_time = 10000;
	//begin noice ui
	uint32_t begin_tick = ControlGetTick();
	uint32_t counter_up = ControlGetTick() - begin_tick;
	e->UI_para.state = ready;
	printf("\r\n vor begin");
	printf("\r\n");
	//启动部分
	while (counter_up < begin_time)
	{
		printf("%d", counter_up);
		//wait inc and camer
		SaftExitDelay(10, e->ExitFlag);
		counter_up = ControlGetTick() - begin_tick;
		e->UI_para.sec = counter_up;
		printf("\r");
	}
	//执行部分
	begin_tick = ControlGetTick();
	counter_up = ControlGetTick() - begin_tick;
	e->UI_para.state = taskruning;
	//motor initial
	while (counter_up < begin_time * 10)
	{
		printf("%d", counter_up);
		//wait motor infinsh
		SaftExitDelay(10, e->ExitFlag);
		counter_up = ControlGetTick() - begin_tick;
		printf("\r");
	}
	printf("\r\n vor end");
	e->UI_para.state = end;
	e->FinishFlag = 1;
}
