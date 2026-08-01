PROJ = SmartRcCar_M7
CPU ?= cortex-m7
BOARD ?= stm32vldiscovery

# Add floating point flags
FPU_FLAGS = -mfpu=fpv5-d16 -mfloat-abi=hard

FREERTOS_PATH = ../../FreeRTOS-Kernel
SRC_FILE_PATH = ../port
ARM_FILES_PATH = ../../arm_cortex
APPLICATION_PATH = ../../application
HAL_DRIVER_PATH = ../../STM32H7xx_HAL_Driver

INC =\
-I$(ARM_FILES_PATH) \
-I$(HAL_DRIVER_PATH)/Inc \
-I$(SRC_FILE_PATH) \
-I$(APPLICATION_PATH)/include \
-I$(FREERTOS_PATH)/include \
-I$(FREERTOS_PATH)/portable/GCC/ARM_CM7/r0p1


FREERTOS_SRC = 	$(FREERTOS_PATH)/tasks.c \
				$(FREERTOS_PATH)/custom_hooks.c \
				$(FREERTOS_PATH)/list.c \
				$(FREERTOS_PATH)/queue.c \
				$(FREERTOS_PATH)/timers.c \
				$(FREERTOS_PATH)/portable/MemMang/heap_4.c \
				$(FREERTOS_PATH)/portable/GCC/ARM_CM7/r0p1/port.c \

# Include all .c files from application source folder
ARM_SRC = $(wildcard $(ARM_FILES_PATH)/*.c)
PORT_SRC = $(wildcard $(SRC_FILE_PATH)/*.c)
APPLICATION_SRC = $(wildcard $(APPLICATION_PATH)/source/*.c)
HAL_SRC = $(wildcard $(HAL_DRIVER_PATH)/Src/*.c)

ALL_SRC =  $(ARM_SRC) $(FREERTOS_SRC) $(APPLICATION_SRC) $(HAL_SRC) $(PORT_SRC)

OBJ = $(SRC_FILE_PATH)/boot.o

OBJ += $(ALL_SRC:.c=.o)


.PHONY: all
all: $(PROJ).elf

%.o: %.S
	arm-none-eabi-as -mthumb -mcpu=$(CPU) -g -c $^ -o $@

%.o: %.c
	arm-none-eabi-gcc $(INC) $(FPU_FLAGS) -mthumb -mcpu=$(CPU) -O0 -g -DCORE_CM7 -DDUAL_CORE -DUSE_PWR_DIRECT_SMPS_SUPPLY -c $^ -o $@

$(PROJ).elf: $(OBJ)
	arm-none-eabi-gcc $(FPU_FLAGS) -mthumb -mcpu=$(CPU) -T Map_stm32h745_M7.ld -Wl,-Map=$(PROJ).map --specs=nosys.specs -Wl,--gc-sections $^ -lm -lc -o $@
	arm-none-eabi-objdump -D -S $@ > $@.lst
	arm-none-eabi-readelf -a $@ > $@.debug

clean:
	-powershell -Command "Remove-Item -Path *.out, *.elf, *.hex, .gdb_history, *.lst, *.debug -Force -ErrorAction SilentlyContinue"
	-powershell -Command "'$(OBJ)'.Split(' ') | ForEach-Object { if ($$_) { Remove-Item -Path $$_ -Force -ErrorAction SilentlyContinue } }"
qemu:
	qemu-system-arm -S -M $(BOARD) -cpu $(CPU) -nographic -kernel $(PROJ).elf -gdb tcp::1234

gdb:
	gdb-multiarch -q -tui $(PROJ).elf -ex "target remote localhost:1234"

