#include "stm32f10x_it.h"

/* Khai báo các biến đếm tĩnh dùng cho SysTick */
static volatile uint32_t u32_count_led1 = 0; /* Cho LED 0.1Hz */
static volatile uint32_t u32_count_led2 = 0; /* Cho LED 1Hz   */
static volatile uint32_t u32_count_led3 = 0; /* Cho LED 10Hz  */

/**
  * @brief  Trình xử lý ngắt SysTick (Tự động gọi mỗi 1ms)
  */
void SysTick_Handler(void)
{
    u32_count_led1++;
    u32_count_led2++;
    u32_count_led3++;

    /* LED 3 (10Hz): Bật 50ms, Tắt 50ms */
    if (u32_count_led3 >= 50)
    {
        GPIOC->ODR ^= GPIO_Pin_15;
        u32_count_led3 = 0;
    }

    /* LED 2 (1Hz): Bật 500ms, Tắt 500ms */
    if (u32_count_led2 >= 500)
    {
        GPIOC->ODR ^= GPIO_Pin_14;
        u32_count_led2 = 0;
    }

    /* LED 1 (0.1Hz): Bật 5000ms (5s), Tắt 5000ms (5s) */
    if (u32_count_led1 >= 5000)
    {
        GPIOC->ODR ^= GPIO_Pin_13;
        u32_count_led1 = 0;
    }
}
