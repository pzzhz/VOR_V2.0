/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-22 15:01:34
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-07-23 11:51:18
 * @FilePath: \USER\main.c
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "sys.h"
#include "delay.h"
#include "lcd.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
//#include "lv_conf.h"
#include "ft5206.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lv_conf.h"


volatile	int idex=0;
int *pt =(int *) 0x20001000;
TaskHandle_t Thread_Lvgl;

void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x0; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x2; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

void LVGL_HANDLER()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
	 LCD_Init();      
	 FT5206_Init();
	 LCD_Display_Dir(1);
	  lv_init();            // lvgl系统初�?��?
    lv_port_disp_init();  // lvgl显示接口初�?��?,放在lv_init()的后�??
    lv_port_indev_init(); // lvgl输入接口初�?��?,放在lv_init()的后�??
	 TIM3_Int_Init(1999, 83); // 定时器初始化(1ms�?�?),用于给lvgl提供1ms的心跳节�??
	lv_btn_create(lv_scr_act());
	while (1)
    {
       touch_sever(1, NULL);
       lv_task_handler();
       vTaskDelay(5);
    }
}

void get_pt()
{
	
*pt=*pt+1;
	
}

int main(void)
{
    delay_init(168);                                // ��ʼ����ʱ��??
		TaskHandle_t handle;
		xTaskCreate((TaskFunction_t)LVGL_HANDLER,
                    (const char *)"Outside_motor",
                    (uint16_t)512,
                    (void *)NULL,
                    (UBaseType_t)2,
                    (TaskHandle_t *)&handle);
		vTaskStartScheduler();
    
}

//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&TIM_IT_Update)//����ж�
	{
		 lv_tick_inc(2);//lvgl��1ms����
	}				   
	TIM3->SR = (uint16_t)~TIM_IT_Update;
}

