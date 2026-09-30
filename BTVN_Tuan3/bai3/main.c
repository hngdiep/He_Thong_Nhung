#include "stm32f1xx.h"
#include "uart_dma.h"
#include "button.h"
#include "delay.h"

#include <stdint.h>


static uint32_t button_value = 0;

static char tx_buffer[64];

static uint8_t button_locked = 0;

static uint8_t message_pending = 0;

static uint16_t pending_length = 0;


static uint16_t Add_String(
    char *buffer,
    uint16_t index,
    const char *str
)
{
    while (*str != '\0')
    {
        buffer[index] = *str;

        index++;
        str++;
    }

    return index;
}


static uint16_t Add_Number(
    char *buffer,
    uint16_t index,
    uint32_t number
)
{
    char temp[10];

    uint8_t i = 0;


    if (number == 0)
    {
        buffer[index] = '0';

        index++;

        return index;
    }


    while (number > 0)
    {
        temp[i] =
            (char)((number % 10U) + '0');

        number /= 10U;

        i++;
    }


    while (i > 0)
    {
        i--;

        buffer[index] = temp[i];

        index++;
    }


    return index;
}


static uint16_t Create_Message(
    char *buffer,
    uint32_t value
)
{
    uint16_t len = 0;


    len = Add_String(
        buffer,
        len,
        "HTN Nhom 1-Nhom 17:BTN:"
    );


    len = Add_Number(
        buffer,
        len,
        value
    );


    buffer[len++] = '\n';
    buffer[len++] = '\r';


    return len;
}


int main(void)
{
    Button_Init();

    UART1_DMA_Init();


    while (1)
    {
        /*
         * Kiem tra DMA da gui xong chua.
         *
         * HAM NAY KHONG CHO.
         */
        UART1_DMA_Service();


        /*
         * ==============================
         * XU LY NUT
         * ==============================
         */
        if (Button_IsPressed())
        {
            if (button_locked == 0)
            {
                /*
                 * Chong doi luc nhan
                 */
                Delay_ms(20);


                if (Button_IsPressed())
                {
                    /*
                     * Khoa lai de giu nut
                     * khong tang lien tuc
                     */
                    button_locked = 1;


                    /*
                     * Tang gia tri
                     */
                    button_value++;


                    /*
                     * Tao ban tin
                     */
                    pending_length =
                        Create_Message(
                            tx_buffer,
                            button_value
                        );


                    /*
                     * Bao co ban tin can gui
                     */
                    message_pending = 1;
                }
            }
        }
        else
        {
            /*
             * Nut dang tha
             */
            if (button_locked)
            {
                /*
                 * Chong doi luc tha
                 */
                Delay_ms(20);


                if (!Button_IsPressed())
                {
                    /*
                     * Cho phep lan nhan moi
                     */
                    button_locked = 0;
                }
            }
        }


        /*
         * ==============================
         * GUI UART BANG DMA
         * ==============================
         */
        if (message_pending)
        {
            /*
             * Neu DMA ranh thi gui.
             *
             * Neu DMA busy,
             * ham return 0 ngay.
             *
             * KHONG while cho.
             */
            if (UART1_DMA_Send(
                    tx_buffer,
                    pending_length))
            {
                message_pending = 0;
            }
        }
    }
}
