#include "stm32f10x.h"

void UART_Init(void)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef uart;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    uart.USART_BaudRate = 9600;
    uart.USART_WordLength = USART_WordLength_8b;
    uart.USART_StopBits = USART_StopBits_1;
    uart.USART_Parity = USART_Parity_No;
    uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &uart);
    USART_Cmd(USART1, ENABLE);
}

char UART_Read(void)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return (char)USART_ReceiveData(USART1);
}

void UART_SendChar(char c)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, c);
}

void UART_SendString(char *s)
{
    while (*s)
        UART_SendChar(*s++);
}

int main(void)
{
    char buffer[128];
    uint16_t index = 0;

    UART_Init();
UART_SendString("STM32 READY\r\n");

    while (1)
    {
        char c = UART_Read();

        if (c == '!')
        {
            buffer[index] = '\0';

            UART_SendString("D23DCDTM0117: ");
            UART_SendString(buffer);
            UART_SendString("\n\r");

            index = 0;
        }
        else if (index < sizeof(buffer) - 1)
        {
            buffer[index++] = c;
        }
    }
}
