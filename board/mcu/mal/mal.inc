#############################################################
# common
#############################################################
SHELL 	        := bash
MKDIR           := mkdir
RM 			        := rm -rf
PYTHON	        := python3

ifeq ("$(V)","1")
  NO_ECHO       :=
else
  NO_ECHO       := @
endif

BUILD_DIR       := _build/
BIN_DIR         := _bin/
LIB_DIR         := _lib/

print-%:
	@echo $* = $($*)

#############################################################
# mal
#############################################################

MAL_MAKE_DIR   	:= $(dir $(lastword $(MAKEFILE_LIST)))

MAL_LIB         := $(MAL_MAKE_DIR)$(LIB_DIR)libmal_$(config).a

LINKER_SCRIPTS  := $(MAL_MAKE_DIR)link.ld

MAL_INCLUDES =  \
  -I$(MAL_MAKE_DIR) \
  -I$(MAL_MAKE_DIR)CMSIS \
  -I$(MAL_MAKE_DIR)CMSIS/Include \
  -I$(MAL_MAKE_DIR)USB_DEVICE/App \
  -I$(MAL_MAKE_DIR)USB_DEVICE/Target \
  -I$(MAL_MAKE_DIR)STM32F4xx_HAL_Driver/Inc \
  -I$(MAL_MAKE_DIR)STM32F4xx_HAL_Driver/Inc/Legacy \
  -I$(MAL_MAKE_DIR)Middlewares/ST/STM32_USB_Device_Library/Core/Inc \
  -I$(MAL_MAKE_DIR)Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc

MAL_FLAGS = \
  -DSTM32F401xC \
  -DUSE_HAL_DRIVER \
  -mcpu=cortex-m4 \
  -mthumb \
  -mfloat-abi=hard \
  -mfpu=fpv4-sp-d16 \
  -fmessage-length=0 \
  -fsigned-char \
  -ffunction-sections \
  -fdata-sections \
  -ffreestanding \
  -fno-builtin \
  -fno-move-loop-invariants \
  -flto \
  -Wall \
  -Wextra

ifeq '$(config)' 'debug'
  MAL_FLAGS +=  \
  -Og \
  -ggdb
else
  MAL_FLAGS +=  \
  -Ofast \
  -g0
endif

MAL_C_FLAGS = \
  $(MAL_FLAGS) \
	-std=c99

MAL_CPP_FLAGS = \
  $(MAL_FLAGS) \
	-std=c++11

MAL_LD_FLAGS = \
  -mcpu=cortex-m4 \
  -mthumb \
  -mfpu=fpv4-sp-d16 \
  -mfloat-abi=hard \
  -flto \
  -T$(LINKER_SCRIPTS) \
  --specs=nano.specs \
  --specs=nosys.specs \
  -Wl,--start-group -lc -lm -lnosys -Wl,--end-group\
  -Wl,-Map=$(BIN_DIR)$(TARGET).map,--cref \
  -Wl,--gc-sections \
  -Wl,--print-memory-usage

# Toolchain commands
TOOLCHAIN_BIN_PATH  := /Users/admin/Desktop/projects/gcc-arm-none-eabi-10.3-2021.10/bin/
LTO_PLUGIN          := /Users/admin/Desktop/projects/gcc-arm-none-eabi-10.3-2021.10/lib/gcc/arm-none-eabi/10.3.1/liblto_plugin.0.so
TOOLCHAIN_PREFIX    := arm-none-eabi
CPP                 := '$(TOOLCHAIN_BIN_PATH)$(TOOLCHAIN_PREFIX)-g++'
CC                  := '$(TOOLCHAIN_BIN_PATH)$(TOOLCHAIN_PREFIX)-gcc'
AS 							    := '$(TOOLCHAIN_BIN_PATH)$(TOOLCHAIN_PREFIX)-gcc'
AR 							    := '$(TOOLCHAIN_BIN_PATH)$(TOOLCHAIN_PREFIX)-ar' --plugin $(LTO_PLUGIN)
OBJDUMP             := '$(TOOLCHAIN_BIN_PATH)$(TOOLCHAIN_PREFIX)-objdump'
OBJCOPY             := '$(TOOLCHAIN_BIN_PATH)$(TOOLCHAIN_PREFIX)-objcopy'
SIZE                := '$(TOOLCHAIN_BIN_PATH)$(TOOLCHAIN_PREFIX)-size'

# Useful commands
ST_CMD            := STM32_Programmer_CLI -c port=SWD
MCU_ERASE         := $(ST_CMD) -e all
MCU_READ          := $(ST_CMD) -r
MCU_WRITE         := $(ST_CMD) -w
MCU_WRITE_SUFFIX  := .elf
MCU_VERIFY        := -v -rst

# JLINK             := JLinkExe
# MCU_ERASE         := $(JLINK) -CommandFile $(MCU_MAKE_DIR)jlink/erase.jlink
# MCU_READ          := $(JLINK)
# MCU_WRITE         := printf "device STM32F401CC\nsi swd\nspeed 4000\nloadbin
# MCU_VERIFY        := ,0x0\nr\ng\nexit" > $(MCU_MAKE_DIR)jlink/flash.jlink; \
#                      $(JLINK) -CommandFile $(MCU_MAKE_DIR)jlink/flash.jlink; \
#                      rm $(MCU_MAKE_DIR)jlink/flash.jlink
# MCU_WRITE_SUFFIX  := .hex

.PHONY: phony

$(MAL_LIB): makemal

makemal: phony
	@echo
	@echo make mal $(config) library
	@echo
	@make -C $(MAL_MAKE_DIR) -f mal.mk all

cleanmal: phony
	@make -C $(MAL_MAKE_DIR) -f mal.mk clean
