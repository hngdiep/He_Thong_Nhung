#include "stm32f1xx.h"

int main(void)
{
    // Bật clock GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // PA0-PA7: Input pull-up
    GPIOA->CRL = 0x88888888;
    GPIOA->ODR |= 0xFF;

    // PA8-PA15: Output push-pull 50MHz
    GPIOA->CRH = 0x33333333;
    while (1)
    {
        int data;

        // Đọc PA0-PA7
        data = GPIOA->IDR & 0xFF;

        // Đảo dữ liệu
        data = (~data) & 0xFF;

        // Ghi lên PA8-PA15
        GPIOA->ODR &= 0x00FF;
        GPIOA->ODR |= data << 8;
    }
}
