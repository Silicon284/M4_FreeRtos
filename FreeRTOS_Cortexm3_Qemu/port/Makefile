PROJ = FreeRTOS
CPU ?= cortex-m3
BOARD ?= stm32vldiscovery

FREERTOS_PATH = ../FreeRTOS-Kernel

INC = 	-I$(FREERTOS_PATH)/include/ \
		-I$(FREERTOS_PATH)/portable/GCC/ARM_CM3/ \
		-I../port/
		

FREERTOS_SRC = 	$(FREERTOS_PATH)/tasks.c \
				$(FREERTOS_PATH)/portable/MemMang/heap_4.c \
				../port/portFunctions.c \
				$(FREERTOS_PATH)/portable/GCC/ARM_CM3/port.c \
				$(FREERTOS_PATH)/list.c \
				$(FREERTOS_PATH)/queue.c \
				$(FREERTOS_PATH)/timers.c
				

OBJ = boot.o\
	start.o

OBJ += $(FREERTOS_SRC:.c=.o)

.PHONY: all
all: $(PROJ).elf

%.o: %.S
	arm-none-eabi-as -mthumb -mcpu=$(CPU) -g -c $^ -o $@

%.o: %.c
	arm-none-eabi-gcc $(INC) -mthumb -mcpu=$(CPU) -O0 -g -c $^ -o $@

$(PROJ).elf: $(OBJ)
	arm-none-eabi-ld -Tmap.ld $^ -o $@
	arm-none-eabi-objdump -D -S $@ > $@.lst
	arm-none-eabi-readelf -a $@ > $@.debug

clean:
	-powershell -Command "Remove-Item -Path *.out, *.elf, .gdb_history, *.lst, *.debug -Force -ErrorAction SilentlyContinue"
	-powershell -Command "'$(OBJ)'.Split(' ') | ForEach-Object { if ($$_) { Remove-Item -Path $$_ -Force -ErrorAction SilentlyContinue } }"
qemu:
	qemu-system-arm -S -M $(BOARD) -cpu $(CPU) -nographic -kernel $(PROJ).elf -gdb tcp::1234

gdb:
	gdb-multiarch -q -tui $(PROJ).elf -ex "target remote localhost:1234"

