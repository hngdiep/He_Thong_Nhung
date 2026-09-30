#include "stm32f10x.h"
#include <stdint.h>

#define ADC_BUFFER_SIZE 100
#define ADC_HALF_SIZE 50

volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];

static void RCC_Config(void)
{
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_ADC1 |
        RCC_APB2Periph_USART1,
        ENABLE
    );

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
}

static void GPIO_Config(void)
{
    GPIO_InitTypeDef gpio;

    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &gpio);

    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio);
}

static void USART1_Config(void)
{
    USART_InitTypeDef usart;

    USART_StructInit(&usart);
    usart.USART_BaudRate = 115200;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode = USART_Mode_Tx;

    USART_Init(USART1, &usart);
    USART_Cmd(USART1, ENABLE);
}

static void DMA_Config(void)
{
    DMA_InitTypeDef dma;
    NVIC_InitTypeDef nvic;

    DMA_DeInit(DMA1_Channel1);

    DMA_StructInit(&dma);
    dma.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    dma.DMA_MemoryBaseAddr = (uint32_t)adc_buffer;
    dma.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma.DMA_BufferSize = ADC_BUFFER_SIZE;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dma.DMA_Mode = DMA_Mode_Circular;
    dma.DMA_Priority = DMA_Priority_High;
    dma.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA1_Channel1, &dma);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_HT | DMA_IT_TC, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    nvic.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&nvic);
    DMA_Cmd(DMA1_Channel1, ENABLE);
}

static void ADC1_Config(void)
{
    ADC_InitTypeDef adc;

    ADC_DeInit(ADC1);

    ADC_StructInit(&adc);
    adc.ADC_Mode = ADC_Mode_Independent;
    adc.ADC_ScanConvMode = DISABLE;
    adc.ADC_ContinuousConvMode = DISABLE;
    adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
    adc.ADC_DataAlign = ADC_DataAlign_Right;
    adc.ADC_NbrOfChannel = 1;

    ADC_Init(ADC1, &adc);

    ADC_RegularChannelConfig(
        ADC1,
        ADC_Channel_0,
        1,
        ADC_SampleTime_55Cycles5
    );

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);

    while (ADC_GetResetCalibrationStatus(ADC1) != RESET)
    {
    }

    ADC_StartCalibration(ADC1);

    while (ADC_GetCalibrationStatus(ADC1) != RESET)
    {
    }

    ADC_ExternalTrigConvCmd(ADC1, ENABLE);
}

static void TIM3_Config(void)
{
    TIM_TimeBaseInitTypeDef timer;

    TIM_TimeBaseStructInit(&timer);
    timer.TIM_Prescaler = 7200 - 1;
    timer.TIM_Period = 100 - 1;
    timer.TIM_CounterMode = TIM_CounterMode_Up;
    timer.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM3, &timer);
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
}

static void USART1_SendByte(uint8_t data)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }

    USART_SendData(USART1, data);
}

static void USART1_SendNumber(uint16_t number)
{
    char digits[5];
    uint8_t length = 0;

    if (number == 0)
    {
        USART1_SendByte('0');
    }
    else
    {
        while (number > 0)
        {
            digits[length] = (char)('0' + number % 10);
            number /= 10;
            length++;
        }

        while (length > 0)
        {
            length--;
            USART1_SendByte((uint8_t)digits[length]);
        }
    }

    USART1_SendByte('\n');
    USART1_SendByte('\r');
}

static void USART1_SendBlock(
    const volatile uint16_t *data,
    uint16_t length
)
{
    uint16_t index;

    for (index = 0; index < length; index++)
    {
        USART1_SendNumber(data[index]);
    }
}

void DMA1_Channel1_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_HT1) != RESET)
    {
        DMA_ClearITPendingBit(DMA1_IT_HT1);
        USART1_SendBlock(&adc_buffer[0], ADC_HALF_SIZE);
    }

    if (DMA_GetITStatus(DMA1_IT_TC1) != RESET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TC1);
        USART1_SendBlock(
            &adc_buffer[ADC_HALF_SIZE],
            ADC_HALF_SIZE
        );
    }
}

int main(void)
{
    RCC_Config();
    GPIO_Config();
    USART1_Config();
    DMA_Config();
    ADC1_Config();
    TIM3_Config();

    TIM_Cmd(TIM3, ENABLE);

    while (1)
    {
        __WFI();
    }
}
