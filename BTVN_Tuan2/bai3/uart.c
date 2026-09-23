#include "uart.h"

void UART1_Init(void)
{
    /*
     * Enable AFIO
     */
    RCC->APB2ENR |= (1 << 0);

    /*
     * Enable GPIOA
     */
    RCC->APB2ENR |= (1 << 2);

    /*
     * Enable USART1
     */
    RCC->APB2ENR |= (1 << 14);


    /*
     * USART1 default:
     *
     * PA9  = TX
     * PA10 = RX
     *
     * Disable USART1 remap
     */
    AFIO->MAPR &= ~(1 << 2);


    /*
     * PA9 = USART1_TX
     *
     * MODE9 = 11  -> Output 50 MHz
     * CNF9  = 10  -> Alternate Function Push Pull
     *
     * 1011 = 0xB
     */
    GPIOA->CRH &= ~(0xF << 4);
    GPIOA->CRH |=  (0xB << 4);


    /*
     * PA10 = USART1_RX
     *
     * MODE10 = 00
     * CNF10  = 01 -> Floating input
     *
     * 0100 = 0x4
     */
    GPIOA->CRH &= ~(0xF << 8);
    GPIOA->CRH |=  (0x4 << 8);


    /* Reset USART */
    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;


    /*
     * PCLK2 = 8 MHz
     * Baud = 115200
     *
     * BRR = 0x45
     */
    USART1->BRR = 0x45;


    /*
     * TE = Transmitter Enable
     */
    USART1->CR1 |= (1 << 3);

    /*
     * RE = Receiver Enable
     */
    USART1->CR1 |= (1 << 2);

    /*
     * UE = USART Enable
     */
    USART1->CR1 |= (1 << 13);
}


void UART1_SendChar(char c)
{
    /*
     * Wait TXE
     */
    while(!(USART1->SR & (1 << 7)))
    {
    }

    USART1->DR = (uint8_t)c;
}


void UART1_SendString(const char *str)
{
    while(*str)
    {
        UART1_SendChar(*str);
        str++;
    }
}


void UART1_SendNumber(uint32_t number)
{
    char buffer[10];

    uint8_t i = 0;

    if(number == 0)
    {
        UART1_SendChar('0');
        return;
    }

    while(number > 0)
    {
        buffer[i] = (number % 10) + '0';

        number /= 10;

        i++;
    }

    while(i > 0)
    {
        i--;

        UART1_SendChar(buffer[i]);
    }
}
