#include "Slave1_Timer.h"
#include "Slave1_IO.h"
#include "stm32f4xx.h"

void Step_Phase_Set(u32 arr, u32 psc);

// void tim_f_sin_set(int f)
// {
//     // #define sys_clk 42000000
//     u8 Mins_flag;
//     int time_arr, time_psc;
//     int a, clk = 84000000, c, y, r = 0, t, i;
//     if (f < 0)
//     {
//         f = -f;
//         Mins_flag = 0;
//     }
//     else
//         Mins_flag = 1;
//     // if (f < 10)
//     //     return;
//     a = clk / (f);
//     c = sqrt(a);
//     y = 10000000;
//     for (i = 2; i <= c; i++)
//     {
//         t = a % (i);

//         if (y > t && a / i < 65536)
//         {

//             y = t;

//             r = i;
//             if (y == 0 && a / i < 65536)
//                 break;
//         }
//     }
//     c = 10;

//     time_arr = a / r - 1;
//     time_psc = r - 1;
//     // tempb = time_arr;
//     // tempc = time_psc;
//     // Out_Put_data = sys_clk / time_arr / time_psc;
//     if (f > 1000 || TIM1->CNT > TIM1->CCR3)
//     {
//         Slave1_Dir_IO(Mins_flag);
//         if (time_arr != TIM1->ARR || time_psc != TIM1->PSC)
//             Step_Phase_Set(time_arr, time_psc);
//     }
//     TIM1->CR1 |= TIM_CR1_CEN;
// }

// void tim_f_set(int f)
// {
//     // #define sys_clk 42000000
//     u8 Mins_flag;
//     int time_arr, time_psc;
//     int a, clk = 84000000, c, y, r = 0, t, i;
//     if (f == 0)
//     {
//         TIM_Cmd(TIM1, 0);
//         return;
//     }
//     if (f < 0)
//     {
//         f = -f;
//         Mins_flag = 0;
//     }
//     else
//         Mins_flag = 1;
//     a = clk / (f);
//     c = sqrt(a);
//     y = 10000000;
//     for (i = 2; i <= c; i++)
//     {
//         t = a % (i);

//         if (y > t && a / i < 65536)
//         {

//             y = t;

//             r = i;
//             if (y == 0 && a / i < 65536)
//                 break;
//         }
//     }
//     c = 10;

//     time_arr = a / r - 1;
//     time_psc = r - 1;
//     // tempb = time_arr;
//     // tempc = time_psc;
//     // Out_Put_data = sys_clk / time_arr / time_psc;
//     if (f > 1000|| TIM1->CNT > TIM1->CCR3)
//     {
//         Slave1_Dir_IO(Mins_flag);
//         if (time_arr != TIM1->ARR || time_psc != TIM1->PSC)
//             Step_Phase_Set(time_arr, time_psc);
//     }
//    TIM1->CR1 |= TIM_CR1_CEN;
// }
float deg;
void Step_Phase_Set(u32 arr, u32 psc)
{
#if 1
    // 此部分需手动修改IO口设置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // TIM_OCInitTypeDef  TIM_OCInitStructure;

    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // 定时器分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseStructure.TIM_Period = arr;                     // 自动重装载值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); // 初始化定时器14
    TIM1->CCR1 = arr / 2;

    TIM_Cmd(TIM1, ENABLE); // 使能TIM14
#else
    TIM1->CR1 &= ~TIM_CR1_CEN;
    uint16_t cval = TIM1->CNT + 1;
    TIM1->PSC = 0;
    TIM1->PSC = psc;
    if (TIM1->CCR1 > TIM1->CNT)
        TIM1->CCR1 = 0xffff;
    else
        TIM1->CCR1 = 0x0;
    deg = (float)TIM1->CNT / TIM1->ARR;
    TIM1->CNT = (arr)*cval / (TIM1->ARR + 1) - 1;
    TIM1->ARR = arr - 1;
    TIM1->CCR1 = arr / 2 - 1;
    TIM1->CR1 |= TIM_CR1_CEN;
#endif
}

void Slave1_Step_Generator_Init(uint32_t arr, uint32_t psc)
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    TIM1->CCR1 = arr / 2;

    GPIOA->MODER &= ~GPIO_MODER_MODER8;
    GPIOA->MODER |= GPIO_MODER_MODER8_1;

    GPIOA->AFR[1] |= 0x1 << (4 * (8 - 8)); // PA8 corresponds to AFR[1] (AFRH), 0th position

    // Set the prescaler value to have TIM1 counter clock = 1 MHz
    TIM1->PSC = 84 - 1; // APB2 clock is 84 MHz

    // Set the ARR value to have a PWM frequency of 1 kHz
    TIM1->ARR = 1000 - 1;

    // Set the CCR1 value to have a duty cycle of 50%
    TIM1->CCR1 = 500;

    // Configure the TIM1 channel 1 in PWM mode 1
    TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;

    // Enable the output compare 1 preload register
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE;

    // Enable the capture/compare 1 output
    TIM1->CCER |= TIM_CCER_CC1E;

    // Enable auto-reload preload
    TIM1->CR1 |= TIM_CR1_ARPE;

    // Enable the TIM1 main output
    TIM1->BDTR |= TIM_BDTR_MOE;

    // Enable the TIM1 counter
    TIM1->CR1 |= TIM_CR1_CEN;
    TIM1->CCR1 = arr / 2;
}

void tim_f_set(int f)
{
    TIM_TypeDef *TIMx = TIM1;
    u8 Mins_flag;
    int time_arr, time_psc;
    int a, clk = 84000000, c, y, r = 0, t, i;
    if (f == 0)
    {
        TIM_Cmd(TIM1, 0);
        return;
    }
    if (f < 0)
    {
        f = -f;
        Mins_flag = 0;
    }
    else
        Mins_flag = 1;
    a = clk / (f);
    c = sqrt(a);
    y = 10000000;
    for (i = 2; i <= c; i++)
    {
        t = a % (i);

        if (y > t && a / i < 65536)
        {

            y = t;

            r = i;
            if (y == 0 && a / i < 65536)
                break;
        }
    }
    c = 10;

    time_arr = a / r - 1;
    time_psc = r - 1;
    // Out_Put_data = sys_clk / time_arr / time_psc;
    if (f > 1000)
    {
        Slave1_Dir_IO(Mins_flag);
        if (time_arr != TIMx->ARR || time_psc != TIMx->PSC)
            Step_Phase_Set(time_arr, time_psc);
    }
    TIMx->CR1 |= TIM_CR1_CEN;
}

void tim_f_sin_set(int f)
{
    TIM_TypeDef *TIMx = TIM1;
    u8 Mins_flag;
    int time_arr, time_psc;
    int a, clk = 84000000, c, y, r = 0, t, i;
    if (f < 0)
    {
        f = -f;
        Mins_flag = 0;
    }
    else
        Mins_flag = 1;
    // if (f < 10)
    //     return;
    a = clk / (f);
    c = sqrt(a);
    y = 10000000;
    for (i = 2; i <= c; i++)
    {
        t = a % (i);

        if (y > t && a / i < 65536)
        {

            y = t;

            r = i;
            if (y == 0 && a / i < 65536)
                break;
        }
    }
    c = 10;

    time_arr = a / r - 1;
    time_psc = r - 1;
    if (f > 1000 || TIMx->CNT > TIMx->CCR3)
    {
        Slave1_Dir_IO(Mins_flag);
        if (time_arr != TIMx->ARR || time_psc != TIMx->PSC)
            Step_Phase_Set(time_arr, time_psc);
    }
    TIMx->CR1 |= TIM_CR1_CEN;
}
