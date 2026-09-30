#include "stm32f10x.h"

#define SSD1306_ADDR 0x78

void I2C1_Config(void)
{
    GPIO_InitTypeDef gpio;
    I2C_InitTypeDef i2c;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &gpio);

    I2C_DeInit(I2C1);

    i2c.I2C_ClockSpeed = 100000;
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c.I2C_OwnAddress1 = 0;
    i2c.I2C_Ack = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    I2C_Init(I2C1, &i2c);
    I2C_Cmd(I2C1, ENABLE);
}

void SSD1306_Write(uint8_t control, uint8_t data)
{
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, SSD1306_ADDR, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, control);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(I2C1, data);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(I2C1, ENABLE);
}

void SSD1306_Command(uint8_t cmd)
{
    SSD1306_Write(0x00, cmd);
}

void SSD1306_Data(uint8_t data)
{
    SSD1306_Write(0x40, data);
}

void SSD1306_Init(void)
{
    SSD1306_Command(0xAE);
    SSD1306_Command(0x20);
    SSD1306_Command(0x00);
    SSD1306_Command(0xB0);
    SSD1306_Command(0xC8);
    SSD1306_Command(0x00);
    SSD1306_Command(0x10);
    SSD1306_Command(0x40);
    SSD1306_Command(0x81);
    SSD1306_Command(0x7F);
    SSD1306_Command(0xA1);
    SSD1306_Command(0xA6);
    SSD1306_Command(0xA8);
    SSD1306_Command(0x3F);
    SSD1306_Command(0xA4);
    SSD1306_Command(0xD3);
    SSD1306_Command(0x00);
    SSD1306_Command(0xD5);
    SSD1306_Command(0x80);
    SSD1306_Command(0xD9);
    SSD1306_Command(0xF1);
    SSD1306_Command(0xDA);
    SSD1306_Command(0x12);
    SSD1306_Command(0xDB);
    SSD1306_Command(0x40);
    SSD1306_Command(0x8D);
    SSD1306_Command(0x14);
    SSD1306_Command(0xAF);
}

void SSD1306_Clear(void)
{
    uint8_t page;
    uint8_t col;

    for (page = 0; page < 8; page++)
    {
        SSD1306_Command(0xB0 + page);
        SSD1306_Command(0x00);
        SSD1306_Command(0x10);

        for (col = 0; col < 128; col++)
            SSD1306_Data(0x00);
    }
}

void SSD1306_Test(void)
{
    uint8_t page;
    uint8_t col;
    uint8_t data;

    SSD1306_Clear();

    for (page = 0; page < 8; page++)
    {
        SSD1306_Command(0xB0 + page);
        SSD1306_Command(0x00);
        SSD1306_Command(0x10);

        for (col = 0; col < 128; col++)
        {
            data = 0x00;

            if ((col >= 28 && col < 100) && (page == 1 || page == 6))
                data = 0xFF;

            if ((col >= 20 && col < 108) && (page >= 2 && page <= 5))
                data = 0x81;

            if ((col >= 36 && col < 44) && (page == 3 || page == 4))
                data = 0xFF;

            if ((col >= 84 && col < 92) && (page == 3 || page == 4))
                data = 0xFF;

            if ((col >= 48 && col < 80) && page == 5)
                data = 0xFF;

            if ((col >= 40 && col < 48) && page == 0)
                data = 0xFF;

            if ((col >= 80 && col < 88) && page == 0)
                data = 0xFF;

            SSD1306_Data(data);
        }
    }
}

int main(void)
{
    SystemInit();
    I2C1_Config();

    for (volatile uint32_t i = 0; i < 1000000; i++);

    SSD1306_Init();
    SSD1306_Test();

    while (1);
}
