#include "pwm.h"

void PWM_TIM2_Init(void)
{
    /* Bat clock GPIOA */
    RCC->APB2ENR |= (1 << 2);

    /* Bat clock AFIO */
    RCC->APB2ENR |= (1 << 0);

    /* Bat clock TIM2 */
    RCC->APB1ENR |= (1 << 0);


    /* TIM2 khong remap
       CH1 = PA0
       CH2 = PA1
       CH3 = PA2
       CH4 = PA3
    */
    AFIO->MAPR &= ~(3 << 8);


    /* PA0 = Alternate Function Push Pull */
    GPIOA->CRL &= ~(0xF << 0);
    GPIOA->CRL |=  (0xB << 0);

    /* PA1 */
    GPIOA->CRL &= ~(0xF << 4);
    GPIOA->CRL |=  (0xB << 4);

    /* PA2 */
    GPIOA->CRL &= ~(0xF << 8);
    GPIOA->CRL |=  (0xB << 8);

    /* PA3 */
    GPIOA->CRL &= ~(0xF << 12);
    GPIOA->CRL |=  (0xB << 12);


    /* ===============================
       PWM = 1 kHz

       Timer clock = 8 MHz

       8 MHz / 8 = 1 MHz

       1 MHz / 1000 = 1 kHz
       =============================== */

    TIM2->PSC = 8 - 1;

    TIM2->ARR = 1000 - 1;


    /* ===============================
       CH1 - PWM Mode 1
       =============================== */

    TIM2->CCMR1 &= ~(7 << 4);
    TIM2->CCMR1 |=  (6 << 4);

    /* Preload CH1 */
    TIM2->CCMR1 |= (1 << 3);


    /* ===============================
       CH2 - PWM Mode 1
       =============================== */

    TIM2->CCMR1 &= ~(7 << 12);
    TIM2->CCMR1 |=  (6 << 12);

    /* Preload CH2 */
    TIM2->CCMR1 |= (1 << 11);


    /* ===============================
       CH3 - PWM Mode 1
       =============================== */

    TIM2->CCMR2 &= ~(7 << 4);
    TIM2->CCMR2 |=  (6 << 4);

    /* Preload CH3 */
    TIM2->CCMR2 |= (1 << 3);


    /* ===============================
       CH4 - PWM Mode 1
       =============================== */

    TIM2->CCMR2 &= ~(7 << 12);
    TIM2->CCMR2 |=  (6 << 12);

    /* Preload CH4 */
    TIM2->CCMR2 |= (1 << 11);


    /* ===============================
       Duty cycle
       =============================== */

    TIM2->CCR1 = 100;    // 10%

    TIM2->CCR2 = 300;    // 30%

    TIM2->CCR3 = 500;    // 50%

    TIM2->CCR4 = 700;    // 70%


    /* Enable CH1 */
    TIM2->CCER |= (1 << 0);

    /* Enable CH2 */
    TIM2->CCER |= (1 << 4);

    /* Enable CH3 */
    TIM2->CCER |= (1 << 8);

    /* Enable CH4 */
    TIM2->CCER |= (1 << 12);


    /* Auto Reload Preload */
    TIM2->CR1 |= (1 << 7);


    /* Update register */
    TIM2->EGR |= (1 << 0);


    /* Start Timer */
    TIM2->CR1 |= (1 << 0);
}
