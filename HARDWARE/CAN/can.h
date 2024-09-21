/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @Date: 2024-08-05 14:28:29
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com
 * @LastEditTime: 2024-09-19 11:15:19
 * @FilePath: \USERd:\workfile\��Ŀ3 vor\software\VOR_V2.0\HARDWARE\CAN\can.h
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/**
 ****************************************************************************************************
 * @file        can.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-01-11
 * @brief       CAN ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F407������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20220111
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef __CAN_H
#define __CAN_H

#include "../SYSTEM/sys/sys.h"

/******************************************************************************************/
/* CAN ���� ���� */

#define CAN_RX_GPIO_PORT GPIOA
#define CAN_RX_GPIO_PIN SYS_GPIO_PIN11
#define CAN_RX_GPIO_AF 9 /* AF����ѡ�� */
#define CAN_RX_GPIO_CLK_ENABLE() \
    do                           \
    {                            \
        RCC->AHB1ENR |= 1 << 0;  \
    } while (0) /* PA��ʱ��ʹ�� */

#define CAN_TX_GPIO_PORT GPIOA
#define CAN_TX_GPIO_PIN SYS_GPIO_PIN12
#define CAN_TX_GPIO_AF 9 /* AF����ѡ�� */
#define CAN_TX_GPIO_CLK_ENABLE() \
    do                           \
    {                            \
        RCC->AHB1ENR |= 1 << 0;  \
    } while (0) /* PA��ʱ��ʹ�� */

/******************************************************************************************/

/* CAN����RX0�ж�ʹ�� */
#define CAN_RX0_INT_ENABLE 1 /* 0,��ʹ��; 1,ʹ��; */

/* ��̬����, ��can.c���Ե��� */
static uint8_t can_pend_msg(uint8_t fifox); /* CAN ��ѯ���� */
static uint8_t can_tx_status(uint8_t mbox); /* CAN ��ѯ����״̬ */
                                            // static uint8_t can_tx_msg(uint32_t id, uint8_t ide, uint8_t rtr, uint8_t len, uint8_t *data);
void can_rx_msg(uint8_t fifox, uint32_t *id, uint8_t *ide, uint8_t *rtr, uint8_t *len, uint8_t *data);

/* ����ӿں��� */
uint8_t can_receive_msg(uint32_t id, uint8_t *buf);                                     /* CAN��������, ��ѯ */
uint8_t can_send_msg(uint32_t id, uint8_t *msg, uint8_t len);                           /* CAN�������� */
uint8_t can_init(uint8_t tsjw, uint8_t tbs2, uint8_t tbs1, uint16_t brp, uint8_t mode); /* CAN��ʼ�� */

extern void (*can_rx_handler)(CanRxMsg *msg);

#endif
