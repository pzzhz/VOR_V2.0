#include "stm32f4xx.h"
#include "math.h"
#include "delay.h"
#include "../HARDWARE/SLAVE/Slave1_IO.h"  
#include "../HARDWARE/SLAVE/Slave1_Timer.h"  
// #include "Slave_motor.h"
// #include "led.h"
// // #include "pwm.h"
// #include "Gui.h"
// #include "DataScope_DP.h"
// #include "usart.h"
// #include "../Motor/TIM_Hardware.h"
// #include "../Motor/Slave_IO_CFG.h"
#define motor_step 10000 * 10
#define angle_step (277.8 * 10)
#define back_speed_gain (10) // 10 ???10???
double Out_Put_data, tempb, tempc;
const float zhengtai[] = {0.0671401181328362, 0.0787266435571731, 0.0921651003682848, 0.107724973016865, 0.125710460380519, 0.146464228763777, 0.170371462141849, 0.197864217263118, 0.229426088805385, 0.265597186890224, 0.306979425865986, 0.354242119344259, 0.408127871992306, 0.469458753526526, 0.539142734707362, 0.618180358900804, 0.707671615951506, 0.808822977724624, 0.922954546746783, 1.05150726095383, 1.19605008869167, 1.35828713888967, 1.54006460182370, 1.74337742621519, 1.97037562869881, 2.22337012207686, 2.50483793942468, 2.81742672219528, 3.16395833218678, 3.54743143979158, 3.97102293456122, 4.43808799902682, 4.95215868314878, 5.51694081497150, 6.13630908326956, 6.81430013042500, 7.55510349869587, 8.36305028063199, 9.24259933485288, 10.1983209418842, 11.2348777913802, 12.3570032119281, 13.5694765777794, 14.8770958532719, 16.2846472653379, 17.7968721272041, 19.4184308720030, 21.1538643932651, 23.0075528298371, 24.9836719752639, 27.0861475356447, 29.3186075048669, 31.6843329713773, 34.1862077155999, 36.8266670010583, 39.6076460044813, 42.5305283698692, 45.5960954079025, 48.8044764943716, 52.1551012487038, 55.6466540953934, 59.2770318264577, 63.0433047912726, 66.9416823406611, 70.9674831443941, 75.1151109848855, 79.3780366045036, 83.7487861493974, 88.2189367089977, 92.7791193975023, 97.4190303619495, 102.127450031335, 106.892270843220, 111.700533599152, 116.538472508835, 121.391568886448, 126.244613361912, 131.081776366588, 135.886686548275, 140.642516665838, 145.332076411009, 149.937911505345, 154.442408325574, 158.827903222264, 163.076795616263, 167.171663886313, 171.095383000717, 174.831242797397, 178.363065780919, 181.675323283072, 184.753248825993, 187.582947534037, 190.151500462857, 192.447062751362, 194.458954554239, 196.177743778858, 197.595319730034, 198.704956858232, 199.501367910180, 199.980745894189, 200.140794394111, 199.980745894189, 199.501367910180, 198.704956858232, 197.595319730034, 196.177743778858, 194.458954554239, 192.447062751362, 190.151500462857, 187.582947534037, 184.753248825993, 181.675323283072, 178.363065780919, 174.831242797397, 171.095383000717, 167.171663886313, 163.076795616263, 158.827903222264, 154.442408325574, 149.937911505345, 145.332076411009, 140.642516665838, 135.886686548275, 131.081776366588, 126.244613361912, 121.391568886448, 116.538472508835, 111.700533599152, 106.892270843220, 102.127450031335, 97.4190303619495, 92.7791193975023, 88.2189367089977, 83.7487861493974, 79.3780366045036, 75.1151109848855, 70.9674831443941, 66.9416823406611, 63.0433047912726, 59.2770318264577, 55.6466540953934, 52.1551012487038, 48.8044764943716, 45.5960954079025, 42.5305283698692, 39.6076460044813, 36.8266670010583, 34.1862077155999, 31.6843329713773, 29.3186075048669, 27.0861475356447, 24.9836719752639, 23.0075528298371, 21.1538643932651, 19.4184308720030, 17.7968721272041, 16.2846472653379, 14.8770958532719, 13.5694765777794, 12.3570032119281, 11.2348777913802, 10.1983209418842, 9.24259933485288, 8.36305028063199, 7.55510349869587, 6.81430013042500, 6.13630908326956, 5.51694081497150, 4.95215868314878, 4.43808799902682, 3.97102293456122, 3.54743143979158, 3.16395833218678, 2.81742672219528, 2.50483793942468, 2.22337012207686, 1.97037562869881, 1.74337742621519, 1.54006460182370, 1.35828713888967, 1.19605008869167, 1.05150726095383, 0.922954546746783, 0.808822977724624, 0.707671615951506, 0.618180358900804, 0.539142734707362, 0.469458753526526, 0.408127871992306, 0.354242119344259, 0.306979425865986, 0.265597186890224, 0.229426088805385, 0.197864217263118, 0.170371462141849, 0.146464228763777, 0.125710460380519, 0.107724973016865, 0.0921651003682848, 0.0787266435571731, 0.0671401181328362};

float sin_data;
int sin_time = 0;
int tim_count, last_count, plus_f = 1;
extern struct data1 State;
#define Pi 3.1415926
void tim_f_sin_set(int f);
void Slave_motor(int Angle_speed, float Feq)
{
    float time_max = 1000;
    int tem = time_max / Feq;
    // if (sin_time % tem <= cam_led_time)
    //     cam_led = 1;
    // else
    //     cam_led = 0;
    sin_data = sin(((float)2 * Pi * Feq * sin_time++ / 1000));
    tim_f_sin_set(angle_step * sin_data * Angle_speed);
    // last_data = sin_time;
}

void VOR_handler(void)
{
    // if (State.motor_run && State.flag)
        Slave_motor(80, 1);
    // else
    // {
    //     if (State.flag == 0 && State.motor_run)
    //         if (slave_sin_back())
    //             State.motor_run = 0;
    // }
}

void motor_handler(void)
{
        VOR_handler();
}


void TIM4_IRQHandler(void)
{
    if (TIM4->SR & TIM_IT_Update) // ????????????
    {
        motor_handler();
    }
    TIM4->SR = (uint16_t)~TIM_IT_Update;
}

void tim_f_set(int f)
{
    // #define sys_clk 42000000
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
    tempb = time_arr;
    tempc = time_psc;
    // Out_Put_data = sys_clk / time_arr / time_psc;
    if (f > 1000)
    {
        Slave1_Dir_IO(Mins_flag);
        if (time_arr != TIM1->ARR || time_psc != TIM1->PSC)
            Step_Phase_Set(time_arr, time_psc);
    }
    TIM1->CR1 |= TIM_CR1_CEN;
}

void tim_f_sin_set(int f)
{
    // #define sys_clk 42000000
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
    tempb = time_arr;
    tempc = time_psc;
    // Out_Put_data = sys_clk / time_arr / time_psc;
    if (f > 1000 || TIM1->CNT > TIM1->CCR3)
    {
        Slave1_Dir_IO(Mins_flag);
        if (time_arr != TIM1->ARR || time_psc != TIM1->PSC)
            Step_Phase_Set(time_arr, time_psc);
    }
    TIM1->CR1 |= TIM_CR1_CEN;
}

/**
 * ??????????????¡ã??¡ã?????¡§360*180???
 * angle_speed ?¡ì?¨¦?????
 * feq         ¨¦?????
 * */
// void Slave_motor(int Angle_speed, float Feq)
// {
//     float time_max = 1000;
//     int tem = time_max / Feq;

//     sin_data = sin(((float)2 * Pi * Feq * sin_time++ / 1000));
//     tim_f_sin_set(angle_step * sin_data * Angle_speed);
//     // last_data = sin_time;
// }
float OutPut;
u8 Slave_Back(int Tag_Pos)
{
    return 1;
// #define P 0.08
#define limit 30000 // ¨¦??¨¦??
    float p = 1000 / 33;
#define Val 400
    OutPut = p * (Tag_Pos - 1000);
    // if(OutPut>1000) OutPut=1000;
    if (OutPut > limit)
        OutPut = limit;
    if (OutPut < -limit)
        OutPut = -limit;
    tim_f_set((int)OutPut);
    Out_Put_data = Tag_Pos - 0;
    if (Out_Put_data < Val && Out_Put_data > -Val)
    {
        TIM_Cmd(TIM1, 0);
        return 1;
    }
    else
        return 0;
}

u8 Slave_Back_spd(int Tag_Pos, int spd)
{
    return 1;
    // #define P 0.08
    float p = 250 / 33;
    spd = spd * back_speed_gain;
#define Val 400
    // static short Last_data;
    float OutPut;
    OutPut = p * (Tag_Pos - 0);
    // if(OutPut>1000) OutPut=1000;
    if (OutPut > spd)
        OutPut = spd;
    if (OutPut < -spd)
        OutPut = -spd;
    tim_f_set((int)OutPut);
    Out_Put_data = Tag_Pos - 0;
    if (Out_Put_data < Val && Out_Put_data > -Val)
    {
        TIM_Cmd(TIM1, 0);
        return 1;
    }
    else
        return 0;
}
u8 slave_sin_back(void)
{
    // return 1;
    // static float Last_data = -1;
    // // if(sin_time%1000)
    // float time_max = 1000;
    // int tem = time_max / State.Frep_VOR;
    // // ????????¡§???
    // if (sin_time % tem == 0)
    // {
    //     if (Slave_Back(0))
    //     {
    //         Last_data = 10; // sin_data ?¡è¡ì?¡ã????-1~1??????¨¦?¡è??¡ã???
    //         sin_time = 0;
    //         return 1;
    //     }
    //     else
    //         return 0;
    // }
    // else
    // {
    //     // tempb = sin_time % tem;
    //     Slave_motor(State.Vel, State.Frep_VOR);
    //     Last_data = sin_data;
    //     return 0;
    // }
}

void Slave_Motor_Vel_Mode(float Vel, u8 clear_fg)
{
    tim_f_set((int)(Vel * angle_step));
}
