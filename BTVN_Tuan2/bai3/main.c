#include "stm32f1xx.h"
#include "adc.h"
#include "uart.h"
#include "systick.h"

#include <stdint.h>

void Clock_HSI8_Init(void)
{
    /* Bat HSI 8 MHz */
    RCC->CR |= (1 << 0);

    while (!(RCC->CR & (1 << 1)));

    /* SYSCLK = HSI */
    RCC->CFGR &= ~(3 << 0);

    while ((RCC->CFGR & (3 << 2)) != 0);

    /* Tat PLL */
    RCC->CR &= ~(1 << 24);

    /* AHB /1 */
    RCC->CFGR &= ~(0xF << 4);

    /* APB1 /1 */
    RCC->CFGR &= ~(7 << 8);

    /* APB2 /1 */
    RCC->CFGR &= ~(7 << 11);
}


int main(void)
{
    uint16_t adc_value;
    uint32_t voltage_mv;

    Clock_HSI8_Init();

    UART1_Init();

    ADC1_Init();

    SysTick_Init();

    UART1_SendString("\r\nLIGHT SENSOR START\r\n");

    while (1)
    {
        /*
         * COUNTFLAG bit 16
         * Sau moi 1 giay se = 1
         */
        if (SysTick->CTRL & (1 << 16))
        {
            /* Doc ADC */
            adc_value = ADC1_Read();

            /*
             * ADC 12 bit: 0 -> 4095
             * Vref = 3.3V = 3300mV
             */
            voltage_mv =
                ((uint32_t)adc_value * 3300U) / 4095U;

            /* Gui gia tri ADC */
            UART1_SendString("ADC = ");

            UART1_SendNumber(adc_value);

            /* Gui dien ap */
            UART1_SendString(" | Voltage = ");

            UART1_SendNumber(voltage_mv);

            UART1_SendString(" mV\r\n");
        }
    }
}
