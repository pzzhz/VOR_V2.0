/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-30 14:37:06
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-08-02 18:28:20
 * @FilePath: \USERd:\workfile\��Ŀ3 vor\software\VOR_V2.0\HARDWARE\USART\USART5.c
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "USART3.h"
#include "stm32f4xx.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"

static void (*Interrupt_handle)(uint8_t dr);

void USART3_INIT(
    uint32_t boundrate,
    void (*handle)(uint8_t dr))
{
    // GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    Interrupt_handle = handle;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  // ʹ��GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // ʹ��UART5ʱ��

    // ����1��Ӧ���Ÿ���ӳ��
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3); // GPIOA9����ΪUART5
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3); // GPIOA10����ΪUART5

    // UART5�˿�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;        //
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      // ���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    // ���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      // ����
    GPIO_Init(GPIOC, &GPIO_InitStructure);            //

    // UART5�˿�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;        // GPIOA9��GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      // ���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    // ���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      // ����
    GPIO_Init(GPIOC, &GPIO_InitStructure);            // ��ʼ��PA9��PA10

    // UART5 ��ʼ������
    USART_InitStructure.USART_BaudRate = boundrate;                                 // ����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // �ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // ����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // �շ�ģʽ
    USART_Init(USART3, &USART_InitStructure);                                       // ��ʼ������1

    USART_Cmd(USART3, ENABLE); // ʹ�ܴ���1

    // #if EN_UART5_RX
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // ��������ж�

    // UART5 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;         // ����1�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // ��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        // �����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           // ����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}
static char printf_buf[100];
static uint16_t send_index, send_len;
uint8_t USART3_FIFO_Send(uint8_t *dr)
{
    if (send_index >= send_len)
    {
        send_index = 0;
        send_len = 0;
        return 0;
    }
    if (send_len)
    {
        *dr = printf_buf[send_index++];
    }
    return 1;
}

uint8_t USART3_Send_Package(char *string, uint16_t size)
{
    if (send_len)
        return 0;
    memcpy(printf_buf, string, size);
    send_len = size;
    USART_ITConfig(USART3, USART_IT_TXE, ENABLE); // ��������ж�
    return 1;
}

uint8_t USART3_PRINTF(char *string, uint16_t size)
{
    if (send_len)
        return 0;
    memcpy(printf_buf, string, size);
    send_len = size;
    USART_ITConfig(USART3, USART_IT_TXE, ENABLE); // ��������ж�
    return 1;
}

void USART3_IRQHandler(void) // ����1�жϷ������
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // �����ж�(���յ������ݱ���??0x0d 0x0a��β)
    {
        uint8_t dr = USART3->DR;
        if (Interrupt_handle != 0)
            Interrupt_handle(dr);
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
    if (USART_GetITStatus(USART3, USART_IT_TXE))
    {
        uint8_t dr;
        if (USART3_FIFO_Send(&dr))
        {
						USART1->DR=dr;
            USART3->DR = dr;
        }
        else
        {
						 USART1->DR=0xaa;
            USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
        }
    }
    USART3->SR = 0;
}