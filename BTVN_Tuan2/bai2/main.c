#include "stm32f10x.h"

/* Khai báo hàm cấu hình */
static void GPIO_Config(void);
static void SysTick_Config_1ms(void);

int main(void)
{
    /* Gọi hàm SystemInit để khởi tạo clock hệ thống */
    SystemInit();

    /* 1. Cấu hình GPIO cho các LED (PC13, PC14, PC15) */
    GPIO_Config();

    /* 2. Cấu hình SysTick tạo ngắt định kỳ mỗi 1ms */
    SysTick_Config_1ms();

    /* 3. Vòng lặp chính */
    while (1)
    {
        /* Mọi logic nháy LED đã được xử lý tự động trong ngắt SysTick */
    }
}

/**
  * @brief  Cấu hình chân PC13, PC14, PC15 làm Output Push-Pull
  */
static void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Bật Clock cho GPIOC */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* Cấu hình các chân PC13, PC14, PC15 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Tắt tất cả các LED ban đầu */
    GPIO_SetBits(GPIOC, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}

/**
  * @brief  Cấu hình SysTick tạo ngắt mỗi 1ms
  */
static void SysTick_Config_1ms(void)
{
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        while (1); // Lỗi cấu hình SysTick
    }
}
