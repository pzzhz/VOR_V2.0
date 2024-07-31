/*
 * @Author: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @Date: 2024-07-30 14:37:06
 * @LastEditors: pzzhh2 101804901+Pzzhh@users.noreply.github.com.
 * @LastEditTime: 2024-07-31 09:24:00
 * @FilePath: \USERd:\workfile\��Ŀ3 vor\software\VOR_V2.0\HARDWARE\USART\USART5.c
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "USART5.h"
#include "stm32f4xx.h"

static void (*Interrupt_handle)(uint8_t dr);

void USART5_INIT(
    uint32_t boundrate,
    void (*handle)(uint8_t dr))
{
    // GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    Interrupt_handle = handle;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE); // ʹ��GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);                        // ʹ��UART5ʱ��

    // ����1��Ӧ���Ÿ���ӳ��
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5); // GPIOA9����ΪUART5
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);  // GPIOA10����ΪUART5

    // UART5�˿�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;        //
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      // ���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    // ���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      // ����
    GPIO_Init(GPIOC, &GPIO_InitStructure);            //

    // UART5�˿�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;         // GPIOA9��GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      // ���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    // ���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      // ����
    GPIO_Init(GPIOD, &GPIO_InitStructure);            // ��ʼ��PA9��PA10

    // UART5 ��ʼ������
    USART_InitStructure.USART_BaudRate = boundrate;                                 // ����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // �ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // ����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // �շ�ģʽ
    USART_Init(UART5, &USART_InitStructure);                                        // ��ʼ������1

    USART_Cmd(UART5, ENABLE); // ʹ�ܴ���1

    // USART_ClearFlag(UART5, USART_FLAG_TC);

    // #if EN_UART5_RX
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); // ��������ж�

    // UART5 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;          // ����1�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // ��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // �����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           // ����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

void UART5_IRQHandler(void) // ����1�жϷ������
{
    if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) // �����ж�(���յ������ݱ���??0x0d 0x0a��β)
    {
        uint8_t dr = UART5->DR;
        if (Interrupt_handle != 0)
            Interrupt_handle(dr);
        USART_ClearITPendingBit(UART5, USART_IT_RXNE);
    }
}