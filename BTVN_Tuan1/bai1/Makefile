CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
CFLAGS = -mcpu=cortex-m3 -mthumb -Wall -O0

all: firmware.bin

firmware.elf: main.c startup.c
	$(CC) $(CFLAGS) -T linker.ld -nostdlib -o $@ $^

firmware.bin: firmware.elf
	$(OBJCOPY) -O binary $< $@

flash: firmware.bin
	st-flash write firmware.bin 0x08000000

clean:
	rm -f *.elf *.bin *.o
