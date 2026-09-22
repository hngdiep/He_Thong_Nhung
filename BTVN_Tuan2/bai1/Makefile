TARGET = bai1

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CFLAGS = -mcpu=cortex-m3 -mthumb -O0 -g
CFLAGS += -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DSYSCLK_FREQ_72MHz=72000000
CFLAGS += -Wall

LIB = STM32F10x_StdPeriph_Lib

INCLUDES = \
-Iinc \
-I$(LIB)/Libraries/CMSIS/CM3/CoreSupport \
-I$(LIB)/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
-I$(LIB)/Libraries/STM32F10x_StdPeriph_Driver/inc

SRC = \
src/main.c \
src/system_stm32f10x.c \
$(LIB)/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c \
$(LIB)/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
$(LIB)/Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c

STARTUP = startup/startup_stm32f10x_md.s

LDFLAGS = -mcpu=cortex-m3 -mthumb
LDFLAGS += -TSTM32F103C8T6.ld
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -specs=nosys.specs

all:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) $(STARTUP) $(LDFLAGS) -o build/$(TARGET).elf
	$(OBJCOPY) -O ihex build/$(TARGET).elf build/$(TARGET).hex
	$(OBJCOPY) -O binary build/$(TARGET).elf build/$(TARGET).bin
	$(SIZE) build/$(TARGET).elf

clean:
	rm -f build/*

