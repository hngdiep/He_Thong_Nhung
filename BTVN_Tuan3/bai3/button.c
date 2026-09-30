#include "button.h"

void Button_Init(void)
{
    /* Bat clock GPIOA */
    RCC->APB2ENR |= (1U << 2);

    /*
     * PA0 = Input Pull-up / Pull-down
     *
     * MODE0 = 00
     * CNF0  = 10
     *
     * 1000 = 0x8
     */
    GPIOA->CRL &= ~(0xFU << 0);
    GPIOA->CRL |=  (0x8U << 0);

    /*
     * ODR0 = 1
     * => Pull-up
     */
    GPIOA->ODR |= (1U << 0);
}

uint8_t Button_IsPressed(void)
{
    /*
     * PA0 = 1: khong nhan
     * PA0 = 0: dang nhan
     */

    if ((GPIOA->IDR & (1U << 0)) == 0)
    {
        return 1;
    }

    return 0;
}
