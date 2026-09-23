#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Biến trạng thái LED & PWM */
volatile uint8_t led_state = 0;          /* 0: OFF, 1: ON */
volatile uint16_t last_pwm_percent = 50; /* PWM mặc định 50% */

/* Buffer lưu chuỗi nhận từ UART */
#define RX_BUFFER_SIZE 64
char rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t rx_index = 0;
volatile uint8_t rx_flag = 0; /* Cờ báo nhận xong 1 lệnh (gặp dấu !) */

void PWM_Init(void);
void USART1_Init(void);
void USART1_SendString(char *str);
void Process_Command(char *cmd);

int main(void)
{
    SystemInit();

    USART1_Init();
    PWM_Init();

    USART1_SendString("STM32 Ready! Gui lenh ket thuc bang !\r\n");

    while (1)
    {
        if (rx_flag)
        {
            Process_Command(rx_buffer);
            rx_index = 0;
            rx_flag = 0;
        }
    }
}

void PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_Cmd(TIM2, ENABLE);
}

void USART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

void USART1_SendString(char *str)
{
    while (*str)
    {
        USART_SendData(USART1, *str++);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
}

void Process_Command(char *cmd)
{
    char response[64];

    if (strcmp(cmd, "ON") == 0)
    {
        led_state = 1;
        TIM_SetCompare1(TIM2, (last_pwm_percent * 1000) / 100);
        USART1_SendString("OK: LED IS ON\r\n");
    }
    else if (strcmp(cmd, "OFF") == 0)
    {
        led_state = 0;
        TIM_SetCompare1(TIM2, 0);
        USART1_SendString("OK: LED IS OFF\r\n");
    }
    else if (strcmp(cmd, "Status") == 0)
    {
        sprintf(response, "State: %s, PWM Config: %d%%\r\n", 
                led_state ? "ON" : "OFF", last_pwm_percent);
        USART1_SendString(response);
    }
    else if (strncmp(cmd, "PWM:", 4) == 0)
    {
        int percent = atoi(cmd + 4);
        if (percent >= 0 && percent <= 100)
        {
            last_pwm_percent = percent;
            if (led_state == 1)
            {
                TIM_SetCompare1(TIM2, (last_pwm_percent * 1000) / 100);
            }
            sprintf(response, "OK: PWM set to %d%%\r\n", last_pwm_percent);
            USART1_SendString(response);
        }
        else
        {
            USART1_SendString("ERROR: Invalid Percent\r\n");
        }
    }
    else
    {
        USART1_SendString("ERROR: Unknown Command\r\n");
    }
}
