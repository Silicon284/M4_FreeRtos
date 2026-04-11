PROJ = FreeRTOS
CPU ?= cortex-m4
BOARD ?= stm32vldiscovery

FREERTOS_PATH = ../FreeRTOS_CortexM4_qemu/FreeRTOS-Kernel
SRC_FILE_PATH = ../FreeRTOS_CortexM4_qemu/port
ARM_FILES_PATH = ../FreeRTOS_CortexM4_qemu/arm_cm4
APPLICATION_PATH = ../FreeRTOS_CortexM4_qemu/application

INC = 	-I$(FREERTOS_PATH)/include/ \
		-I$(FREERTOS_PATH)/portable/GCC/ARM_CM3/ \
		-I$(FREERTOS_PATH)/portable/GCC/ARM_CM3/ \
		-I$(ARM_FILES_PATH)/ \
		-I$(SRC_FILE_PATH)/ \
		-I$(APPLICATION_PATH)/include
		

FREERTOS_SRC = 	$(FREERTOS_PATH)/tasks.c \
				$(FREERTOS_PATH)/custom_hooks.c \
				$(FREERTOS_PATH)/list.c \
				$(FREERTOS_PATH)/queue.c \
				$(FREERTOS_PATH)/timers.c \
				$(FREERTOS_PATH)/portable/MemMang/heap_4.c \
				$(FREERTOS_PATH)/portable/GCC/ARM_CM3/port.c \
				$(SRC_FILE_PATH)/portFunctions.c \
				$(SRC_FILE_PATH)/exception_handlers.c \
				$(APPLICATION_PATH)/source/main.c \


OBJ = $(SRC_FILE_PATH)/boot.o\
	  $(SRC_FILE_PATH)/start.o

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

