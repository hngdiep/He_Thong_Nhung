#include "uart_dma.h"

static volatile uint8_t uart_dma_busy = 0;


void UART1_DMA_Init(void)
{
    /* GPIOA clock */
    RCC->APB2ENR |= (1U << 2);

    /* USART1 clock */
    RCC->APB2ENR |= (1U << 14);

    /* DMA1 clock */
    RCC->AHBENR |= (1U << 0);


    /* PA9 = USART1_TX
       Alternate Function Push-Pull */
    GPIOA->CRH &= ~(0xFU << 4);
    GPIOA->CRH |=  (0xBU << 4);


    /* PA10 = USART1_RX
       Input Floating */
    GPIOA->CRH &= ~(0xFU << 8);
    GPIOA->CRH |=  (0x4U << 8);


    /* USART1
       Clock = 8 MHz
       Baud = 115200 */
    USART1->BRR = 0x45;

    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;


    /* RE */
    USART1->CR1 |= (1U << 2);

    /* TE */
    USART1->CR1 |= (1U << 3);

    /* DMAT */
    USART1->CR3 |= (1U << 7);

    /* UE */
    USART1->CR1 |= (1U << 13);


    /* DMA1 Channel 4 = USART1_TX */
    DMA1_Channel4->CCR = 0;


    /* Peripheral = USART1->DR */
    DMA1_Channel4->CPAR =
        (uint32_t)&USART1->DR;


    /* DIR = 1
       Memory -> Peripheral */
    DMA1_Channel4->CCR |= (1U << 4);


    /* MINC = 1 */
    DMA1_Channel4->CCR |= (1U << 7);


    /* Priority Medium */
    DMA1_Channel4->CCR |= (1U << 12);


    /* Clear flags Channel 4 */
    DMA1->IFCR = (0xFU << 12);


    uart_dma_busy = 0;
}


void UART1_DMA_Service(void)
{
    /*
     * TCIF4 = bit 13
     *
     * Chi KIEM TRA, khong cho.
     */
    if (DMA1->ISR & (1U << 13))
    {
        /*
         * Disable DMA
         */
        DMA1_Channel4->CCR &= ~(1U << 0);

        /*
         * Clear Channel 4 flags
         */
        DMA1->IFCR = (0xFU << 12);

        /*
         * Cho phep gui lan sau
         */
        uart_dma_busy = 0;
    }


    /*
     * TEIF4 = bit 15
     */
    if (DMA1->ISR & (1U << 15))
    {
        DMA1_Channel4->CCR &= ~(1U << 0);

        DMA1->IFCR = (0xFU << 12);

        uart_dma_busy = 0;
    }
}


uint8_t UART1_DMA_IsBusy(void)
{
    return uart_dma_busy;
}


uint8_t UART1_DMA_Send(
    char *data,
    uint16_t len
)
{
    /*
     * Neu DMA dang gui
     * return ngay
     *
     * KHONG while cho
     */
    if (uart_dma_busy)
    {
        return 0;
    }


    if ((data == 0) || (len == 0))
    {
        return 0;
    }


    /* Disable DMA */
    DMA1_Channel4->CCR &= ~(1U << 0);


    /* Clear flags */
    DMA1->IFCR = (0xFU << 12);


    /* Memory address */
    DMA1_Channel4->CMAR =
        (uint32_t)data;


    /* Number of bytes */
    DMA1_Channel4->CNDTR =
        len;


    uart_dma_busy = 1;


    /* Enable DMA */
    DMA1_Channel4->CCR |= (1U << 0);


    return 1;
}
