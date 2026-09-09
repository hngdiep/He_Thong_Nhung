#include "stm32f1xx.h"

void delay(int count)
{
    while (count--)
    {
        __asm volatile ("nop");
    }
}

int main(void)
{
    /* Enable GPIOA clock */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    /* PA0 -> PA7: output push-pull, 2 MHz */
    GPIOA->CRL = 0x22222222;

    while (1)
    {
        /* PA0 -> PA7 */
        for (int i = 0; i < 8; i++)
        {
            GPIOA->BSRR = (1 << i);
            delay(1000000);
        }

        /* PA6 -> PA0 */
        for (int i = 7; i >= 0; i--)
        {
            GPIOA->BRR = (1 << i);
            delay(1000000);
        }
    }
}
