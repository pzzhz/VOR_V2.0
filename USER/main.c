/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-22 15:01:34
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-07-24 14:41:48
 * @FilePath: \USER\main.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "sys.h"
#include "delay.h"
#include "lcd.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
// #include "lv_conf.h"
#include "ft5206.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lv_conf.h"
#include "../INTERFACE/UI/other/system_function.h"

volatile int idex = 0;
int *pt = (int *)0x20001000;
TaskHandle_t Thread_Lvgl;

void TIM3_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); /// 使能TIM3时钟

    TIM_TimeBaseInitStructure.TIM_Period = arr;                     // 自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;                  // 定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure); // 初始化TIM3

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // 允许定时器3更新中断
    TIM_Cmd(TIM3, ENABLE);                     // 使能定时器3

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;             // 定时器3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0; // 抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x2;        // 子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

extern void Task_Stroage_Init();
extern void controlInit();
extern void mainpage_init(lv_obj_t *parent);
extern void UI_Page_Management_Init();

void LVGL_HANDLER()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    /*hardware*/
    LCD_Init();
    FT5206_Init();
    LCD_Display_Dir(1);
    lv_init();               // lvgl绯荤粺鍒濆?嬶拷?
    lv_port_disp_init();     // lvgl鏄剧ず鎺ュ彛鍒濆?嬶拷?,鏀惧湪lv_init()鐨勫悗锟??
    lv_port_indev_init();    // lvgl杈撳叆鎺ュ彛鍒濆?嬶拷?,鏀惧湪lv_init()鐨勫悗锟??
    TIM3_Int_Init(1999, 83); // 瀹氭椂鍣ㄥ垵濮嬪寲(1ms涓?鏂?),鐢ㄤ簬缁檒vgl鎻愪緵1ms鐨勫績璺宠妭锟??
    /*system init */
    controlInit();
    Task_Stroage_Init();
    UI_Page_Management_Init();
    while (1)
    {
        touch_sever(1, NULL);
        lv_task_handler();
        ControlDelay(2);
    }
}

void get_pt()
{

    *pt = *pt + 1;
}

int main(void)
{
    delay_init(168); // 初始化延时函??
    TaskHandle_t handle;
    xTaskCreate((TaskFunction_t)LVGL_HANDLER,
                (const char *)"Outside_motor",
                (uint16_t)4096,
                (void *)NULL,
                (UBaseType_t)3,
                (TaskHandle_t *)&handle);
                
    vTaskStartScheduler();
}

// 定时器3中断服务程序
void TIM3_IRQHandler(void)
{
    if (TIM3->SR & TIM_IT_Update) // 溢出中断
    {
        lv_tick_inc(2); // lvgl的1ms心跳
    }
    TIM3->SR = (uint16_t)~TIM_IT_Update;
}
