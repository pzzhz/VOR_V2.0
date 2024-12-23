/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-16 09:37:35
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @LastEditTime: 2024-09-21 14:26:29
 * @FilePath: \USERd:\workfile\项目11 robomaster\software\01 code\ovor\template\c610\C610.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE10
 */
#include "C610.h"
#include "../c610/pid.h"
#include "../CAN/can.h"
#include "stm32f4xx.h"
// #include "Gui.h"

static motor_measure_t motor_chassis[7];
extern struct data1 State;

#define get_motor_measure(ptr, data)                                   \
    {                                                                  \
        (ptr)->last_ecd = (ptr)->ecd;                                  \
        (ptr)->ecd = (uint16_t)((data)[0] << 8 | (data)[1]);           \
        (ptr)->speed_rpm = (uint16_t)((data)[2] << 8 | (data)[3]);     \
        (ptr)->given_current = (uint16_t)((data)[4] << 8 | (data)[5]); \
        (ptr)->temperate = (data)[6];                                  \
    }

float SetSpeed = 0, SetCurrent;
// float TargetCurrent, MotorPower;
pid_t Vpid, Ipid;
motor_measure_t *mymotor;
C610_Ctrl rotate;

void C610_Current_Cmd(C610_Current_Ctrl buf)
{
    can_send_msg(0x200, (uint8_t *)&buf, 8);
    // can_tx_msg(0x200, 0, 0, 8, (uint8_t *)&buf);
}

void C610_Current_Cmd2(int16_t a, int16_t b, int16_t c, int16_t d)
{
    uint8_t buf[8];
    buf[0] = a >> 8;
    buf[1] = a & 0x00ff;
    buf[2] = b >> 8;
    buf[3] = b & 0x00ff;
    buf[4] = c >> 8;
    buf[5] = c & 0x00ff;
    buf[6] = d >> 8;
    buf[7] = d & 0x00ff;
    can_tx_msg(0x200, 0, 0, 8, (uint8_t *)&buf);
}

void Motor_Spd_Pid_INIT()
{
    int ch = 1;
    for (int i = 0; i < 4; i++)
    {
        PID_struct_init(&rotate.pid[i].V, POSITION_PID, 1500, 500, 10, 0.2f, 5.0f);    // 速度
        PID_struct_init(&rotate.pid[i].I, POSITION_PID, 1500, 500, 1.0f, 0.05f, 0.0f); // 电流环
        /* code */
    }
}

int16_t C610_PID_CAL(C610_Pid *PID, motor_measure_t *m, float Speed)
{
    float TargetCurrent, MotorPower;
    if (Speed <= 1 && Speed >= -1)
        return 0;
    TargetCurrent = pid_calc(&PID->V, m->speed_rpm, Speed);
    MotorPower = pid_calc(&PID->I, m->given_current, TargetCurrent);
    return (int16_t)MotorPower;
}
float factor = 1.75;
void Motor_Spd_Pid(float speed)
{
    if (speed == 0)
    {
        C610_Current_Cmd2(0, 0, 0, 0);
        return;
    }
    int16_t pwm;
    float C610_Speed = -speed * 60.0f * 36.0f / factor;
    pwm = C610_PID_CAL(&rotate.pid[0], &motor_chassis[0], speed);
    C610_Current_Cmd2(pwm, 0, 0, 0);
}

float Avarage(float *val, int len)
{
    float res = 0;
    for (int i = 0; i < len; i++)
    {
        res += val[i];
    }
    res /= 10;
    return res;
}

void C610_RX_handler(uint32_t id, uint8_t *msg)
{
    static float Vfilter[4][10];
    static float Ifilter[4][10];
    static int index;
    switch (id)
    {
    case CAN_3508_M1_ID:
    case CAN_3508_M2_ID:
    case CAN_3508_M3_ID:
    case CAN_3508_M4_ID:
    case CAN_YAW_MOTOR_ID:
    case CAN_PIT_MOTOR_ID:
    case CAN_TRIGGER_MOTOR_ID:
    {
        static uint8_t i = 0;
        // get motor id
        i = id - CAN_3508_M1_ID;
        if (index >= 10)
            index = 0;
        if (i < 4)
        {
            get_motor_measure(&motor_chassis[i], msg);
            Vfilter[i][index++] = motor_chassis[i].speed_rpm;
            Ifilter[i][index++] = motor_chassis[i].given_current;
            motor_chassis[i].speed_rpm = Avarage(Vfilter[i], 10);
            motor_chassis[i].given_current = Avarage(Ifilter[i], 10);
        }
        break;
    }

    default:
    {
        break;
    }
    }
}

/**
 * @brief       CAN RX0 中断服务函数
 *   @note      处理CAN FIFO0的接收中断
 * @param       无
 * @retval      无
 */
void C610_CAN_RX_HANDLER(void)
{
    uint8_t rxbuf[8];
    uint32_t id;
    uint8_t ide, rtr, len;
    can_rx_msg(0, &id, &ide, &rtr, &len, rxbuf);
    C610_RX_handler(id, rxbuf);
}
/**
 * @brief       设置NVIC分组
 * @param       group: 0~4,共5组, 详细解释见: sys_nvic_init函数参数说明
 * @retval      无
 */
static void sys_nvic_priority_group_config(uint8_t group)
{
    uint32_t temp, temp1;
    temp1 = (~group) & 0x07; /* 取后三位 */
    temp1 <<= 8;
    temp = SCB->AIRCR;  /* 读取先前的设置 */
    temp &= 0X0000F8FF; /* 清空先前分组 */
    temp |= 0X05FA0000; /* 写入钥匙 */
    temp |= temp1;
    SCB->AIRCR = temp; /* 设置分组 */
}

void sys_nvic_init(uint8_t pprio, uint8_t sprio, uint8_t ch, uint8_t group)
{
    uint32_t temp;
    sys_nvic_priority_group_config(group); /* 设置分组 */
    temp = pprio << (4 - group);
    temp |= sprio & (0x0f >> group);
    temp &= 0xf;                           /* 取低四位 */
    NVIC->ISER[ch / 32] |= 1 << (ch % 32); /* 使能中断位(要清除的话,设置ICER对应位为1即可) */
    NVIC->IP[ch] |= temp << 4;             /* 设置响应优先级和抢断优先级 */
}

void C610_Ctrl_Init()
{
    can_rx_handler = C610_CAN_RX_HANDLER;
    Motor_Spd_Pid_INIT();
}