
#include <stdint.h>
#include "stm32f1xx.h"
#include "FreeRTOS.h"
#include "task.h"

#define LED1_PIN  0
#define LED2_PIN  1
#define LED3_PIN  2

typedef struct
{
    uint8_t pin;
    float frequency;
} LED_Config;

static LED_Config led1 = {LED1_PIN, 0.1f};
static LED_Config led2 = {LED2_PIN, 1.0f};
static LED_Config led3 = {LED3_PIN, 10.0f};

void LED_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // PA0, PA1, PA2: Output Push-Pull, 2 MHz
    GPIOA->CRL &= ~0x00000FFF;
    GPIOA->CRL |=  0x00000222;

    // Tat 3 LED ban dau
    GPIOA->BRR = (1U << LED1_PIN) |
                 (1U << LED2_PIN) |
                 (1U << LED3_PIN);
}

// Ham dieu khien LED dung chung
void BlinkLED(uint8_t pin, float frequency)
{
    uint32_t halfPeriodMs =
        (uint32_t)(500.0f / frequency + 0.5f);

    TickType_t halfPeriod =
        pdMS_TO_TICKS(halfPeriodMs);

    if (halfPeriod == 0)
    {
        halfPeriod = 1;
    }

    TickType_t lastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        // Bat LED
        GPIOA->BSRR = (1U << pin);

        xTaskDelayUntil(&lastWakeTime, halfPeriod);

        // Tat LED
        GPIOA->BRR = (1U << pin);

        xTaskDelayUntil(&lastWakeTime, halfPeriod);
    }
}

// Ham thuc thi chung cho 3 task
void LED_Task(void *pvParameters)
{
    LED_Config *config = (LED_Config *)pvParameters;

    BlinkLED(config->pin, config->frequency);
}

int main(void)
{
    LED_Init();

    BaseType_t task1, task2, task3;

    task1 = xTaskCreate(
        LED_Task,
        "LED_0.1Hz",
        128,
        &led1,
        1,
        NULL
    );

    task2 = xTaskCreate(
        LED_Task,
        "LED_1Hz",
        128,
        &led2,
        1,
        NULL
    );

    task3 = xTaskCreate(
        LED_Task,
        "LED_10Hz",
        128,
        &led3,
        1,
        NULL
    );

    if (task1 != pdPASS ||
        task2 != pdPASS ||
        task3 != pdPASS)
    {
        while (1);
    }

    // Khoi dong FreeRTOS Scheduler
    vTaskStartScheduler();

    while (1);
}
