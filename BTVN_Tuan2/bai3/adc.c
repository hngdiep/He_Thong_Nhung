#include "adc.h"

void ADC1_Init(void)
{
    volatile uint32_t i;

    /* Bat clock GPIOA */
    RCC->APB2ENR |= (1 << 2);

    /* Reset ADC1 */
    RCC->APB2RSTR |= (1 << 9);
    RCC->APB2RSTR &= ~(1 << 9);

    /* Bat clock ADC1 */
    RCC->APB2ENR |= (1 << 9);

    /*
     * ADC clock = PCLK2 / 2
     * PCLK2 = 8 MHz
     * ADC clock = 4 MHz
     */
    RCC->CFGR &= ~(3 << 14);

    /*
     * PA0 = Analog Input
     * MODE0 = 00
     * CNF0  = 00
     */
    GPIOA->CRL &= ~(0xF << 0);

    /* ADC independent mode */
    ADC1->CR1 = 0;

    /* Reset CR2 */
    ADC1->CR2 = 0;

    /*
     * Sample time channel 0
     * SMP0 = 111
     * 239.5 cycles
     *
     * De test cam bien thi de sample dai cho chac
     */
    ADC1->SMPR2 &= ~(7 << 0);
    ADC1->SMPR2 |=  (7 << 0);

    /*
     * Chi 1 conversion
     */
    ADC1->SQR1 = 0;

    /*
     * Conversion dau tien = ADC channel 0
     * PA0 = ADC1_IN0
     */
    ADC1->SQR3 = 0;

    /*
     * Bat ADC lan 1
     * -> Power ON ADC
     */
    ADC1->CR2 |= (1 << 0);

    /*
     * Cho ADC on dinh
     */
    for(i = 0; i < 1000; i++)
    {
        __NOP();
    }

    /*
     * Reset calibration
     */
    ADC1->CR2 |= (1 << 3);

    while(ADC1->CR2 & (1 << 3))
    {
    }

    /*
     * Start calibration
     */
    ADC1->CR2 |= (1 << 2);

    while(ADC1->CR2 & (1 << 2))
    {
    }
}


uint16_t ADC1_Read(void)
{
    uint32_t timeout = 100000;

    /*
     * Clear EOC
     */
    ADC1->SR &= ~(1 << 1);

    /*
     * ADC da ON roi.
     * Ghi ADON = 1 lan nua de start conversion.
     */
    ADC1->CR2 |= (1 << 0);

    /*
     * Cho EOC = 1
     */
    while(!(ADC1->SR & (1 << 1)))
    {
        timeout--;

        if(timeout == 0)
        {
            /*
             * 65535 = ADC loi / khong conversion duoc
             */
            return 0xFFFF;
        }
    }

    return (uint16_t)(ADC1->DR & 0x0FFF);
}
