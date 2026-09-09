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
    // Bật clock GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // PA0: Input pull-up
    GPIOA->CRL &= ~(0xF << 0);
    GPIOA->CRL |= (0x8 << 0);
    GPIOA->ODR |= (1 << 0);

    // PA3: Output push-pull 2MHz
    GPIOA->CRL &= ~(0xF << 12);
    GPIOA->CRL |= (0x2 << 12);

    // LED ban đầu tắt
    GPIOA->ODR &= ~(1 << 3);

    while (1)
    {
        // Kiểm tra nút được nhấn
        if ((GPIOA->IDR & (1 << 0)) == 0)
        {
            delay(20000);

            // Chờ nhả nút
            while ((GPIOA->IDR & (1 << 0)) == 0)
            {
            }

            delay(20000);

            // Đảo trạng thái LED PA3
            GPIOA->ODR ^= (1 << 3);
        }
    }
}
