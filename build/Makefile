PROJ = SmartRcCar
CPU ?= cortex-m4
BOARD ?= stm32vldiscovery

# Add floating point flags
FPU_FLAGS = -mfpu=fpv4-sp-d16 -mfloat-abi=hard

FREERTOS_PATH = ../Single_Core_SmartCar/FreeRTOS-Kernel
SRC_FILE_PATH = ../Single_Core_SmartCar/port
ARM_FILES_PATH = ../Single_Core_SmartCar/arm_cm4
APPLICATION_PATH = ../Single_Core_SmartCar/application
HAL_DRIVER_PATH = ../Single_Core_SmartCar/STM32H7xx_HAL_Driver

INC =\
-I$(ARM_FILES_PATH) \
-I$(HAL_DRIVER_PATH)/Inc \
-I$(SRC_FILE_PATH) \
-I$(APPLICATION_PATH)/include \
-I$(FREERTOS_PATH)/include \
-I$(FREERTOS_PATH)/portable/GCC/ARM_CM4F


FREERTOS_SRC = 	$(FREERTOS_PATH)/tasks.c \
				$(FREERTOS_PATH)/custom_hooks.c \
				$(FREERTOS_PATH)/list.c \
				$(FREERTOS_PATH)/queue.c \
				$(FREERTOS_PATH)/timers.c \
				$(FREERTOS_PATH)/portable/MemMang/heap_4.c \
				$(FREERTOS_PATH)/portable/GCC/ARM_CM4F/port.c \


# Include all .c files from applicatportion source folder
PORT_SRC = $(wildcard $(SRC_FILE_PATH)/*.c)
# Include all .c files from application source folder
APPLICATION_SRC = $(wildcard $(APPLICATION_PATH)/source/*.c)
# Include all .c files from multiple directories
HAL_SRC = $(wildcard $(HAL_DRIVER_PATH)/Src/*.c)
# Add all application sources
ALL_SRC = $(FREERTOS_SRC) $(APPLICATION_SRC) $(HAL_SRC) $(PORT_SRC)

OBJ = $(SRC_FILE_PATH)/boot.o

OBJ += $(ALL_SRC:.c=.o)


.PHONY: all
all: $(PROJ).elf

%.o: %.S
	arm-none-eabi-as -mthumb -mcpu=$(CPU) -g -c $^ -o $@

%.o: %.c
	arm-none-eabi-gcc $(INC) $(FPU_FLAGS) -mthumb -mcpu=$(CPU) -O0 -g -c $^ -o $@

$(PROJ).elf: $(OBJ)
	arm-none-eabi-gcc $(FPU_FLAGS) -mthumb -mcpu=$(CPU) -T map.ld --specs=nosys.specs -Wl,--gc-sections $^ -lm -lc -o $@
	arm-none-eabi-objdump -D -S $@ > $@.lst
	arm-none-eabi-readelf -a $@ > $@.debug

clean:
	-powershell -Command "Remove-Item -Path *.out, *.elf, .gdb_history, *.lst, *.debug -Force -ErrorAction SilentlyContinue"
	-powershell -Command "'$(OBJ)'.Split(' ') | ForEach-Object { if ($$_) { Remove-Item -Path $$_ -Force -ErrorAction SilentlyContinue } }"
qemu:
	qemu-system-arm -S -M $(BOARD) -cpu $(CPU) -nographic -kernel $(PROJ).elf -gdb tcp::1234

gdb:
	gdb-multiarch -q -tui $(PROJ).elf -ex "target remote localhost:1234"

