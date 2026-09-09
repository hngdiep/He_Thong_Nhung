#include <stdint.h>

#define RCC_APB2ENR (*(volatile uint32_t *)(0x40021000 + 0x18))
#define GPIOC_CRH   (*(volatile uint32_t *)(0x40011000 + 0x04))
#define GPIOC_ODR   (*(volatile uint32_t *)(0x40011000 + 0x0C))

// Trễ ước lượng theo xung nội 8MHz HSI
void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 800; i++) {
        __asm__("nop");
    }
}

int main(void) {
    // 1. Cấp xung nhịp cho GPIOC (bit 4)
    RCC_APB2ENR |= (1 << 4);

    // 2. PC13 là Output Push-Pull 2MHz: MODE13=10, CNF13=00 (bit 20-23)
    GPIOC_CRH &= ~(0xF << 20);
    GPIOC_CRH |=  (0x2 << 20);

    // Tham số chu kỳ: 1000ms = 1 giây (thay đổi giá trị này để đổi chu kỳ)
    uint32_t blink_period_ms = 1000;

    while (1) {
        GPIOC_ODR ^= (1 << 13); // Đảo trạng thái LED PC13
        delay_ms(blink_period_ms);
    }

    return 0;
}
