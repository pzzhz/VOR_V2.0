#include "control.h"
#include "stdint.h"

#define use_windows
#ifndef STM32F40_41xxx
#include "Windows.h"
#include <stdio.h>
#endif // use_windows

void thread_create(void* function, Mode_info_struct* e);

UI_Function_struct control_cb_array[control_cb_array_size];
uint8_t Control_Set_cb(UI_Function_struct* function_array,
    uint16_t index,
    UI_function cb)
{
    for (int i = 0; i < index; i++)
    {
        if (function_array[i].cb == 0)
        {
            function_array[i].cb = cb;
            return 1;
        }
    }
    return 0;
}

uint8_t Control_Remove_cb(UI_Function_struct* function_array,
    uint16_t index,
    UI_function cb)
{
    for (int i = 0; i < index; i++)
    {
        if (function_array[i].cb == cb)
        {
            function_array[i].cb = 0;
            return 1;
        }
    }
    return 0;
}

void control_run_cb_array(UI_Function_struct* function_array,
    uint16_t index,
    char cmd[50],
    char message[50],
    Task_Parameter_Struct* e)
{
    for (int i = 0; i < index; i++)
    {
        if (function_array[i].cb != 0)
        {
            uint8_t res = function_array[i].cb(message, cmd, e);
            /*if (function_array[i].cb(message, cmd, e))*/
                /*return;*/
        }
    }
}
//typedef struct
//{
//
//}controlStruct;


void SaftExitDelay(uint32_t ms, Mode_info_struct* e)
{
    if (e->ExitFlag)
    {
#ifndef STM32F40_41xxx
        ExitThread(0);
#endif // use_windows
    }
    Sleep(ms);
}

uint32_t  ControlGetTick()
{
    uint32_t tick = GetTickCount();
    return tick;
}



void controlfunction()
{
    extern void VorControlFunction(Mode_info_struct * e);
    Mode_info_struct e = { 0 };
    Task_Parameter_Struct info;
    thread_create(VorControlFunction, &e);
    char message[50], cmd[50] = { "" };
    memset(control_cb_array, 0, sizeof(control_cb_array));
    while (1)
    {
        //UI 信息发送
        //UI 按键监听
        //UI Table 按键监听和处理
        //串口 监听
        //串口 反馈
        //紧急停机
        control_run_cb_array(control_cb_array, control_cb_array_size, cmd, message, &info);     //给ui发消息？
        Sleep(10);
        //e.ExitFlag = 1;
    }
}

void thread_create(void* function, Mode_info_struct* e)
{
    HANDLE hThread;
    DWORD dwThreadId;
    hThread = CreateThread(NULL,               // 默认安全属性
        0,                  // 默认堆栈大小
        function,   // 线程函数
        e,          // 传递给线程函数的参数
        0,                  // 默认创建标志
        &dwThreadId);        // 线程ID)
}

void controlInit()
{
#ifndef STM32F40_41xxx
    thread_create(controlfunction, 0);

#endif // use_windows
}


