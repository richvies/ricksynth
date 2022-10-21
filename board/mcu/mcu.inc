# MCU Library

# Common
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

# File directories
MCU_MAKE_DIR    := $(dir $(lastword $(MAKEFILE_LIST)))
MCU_LIB         := $(MCU_MAKE_DIR)$(LIB_DIR)libmcu_$(config).a
MCU_MAKEFILE    := mcu.mk

MAL_DIR         := $(MCU_MAKE_DIR)mal/
MAL_MAKE_DIR    := $(MAL_DIR)
MAL_LIB         := $(MAL_MAKE_DIR)$(LIB_DIR)libmal_$(config).a
MAL_MAKEFILE    := mal.mk

MCU_LIBS        := $(MCU_LIB) $(MAL_LIB)

LINKER_SCRIPTS  := $(MAL_MAKE_DIR)link.ld

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
#ST_CMD            := STM32_Programmer_CLI -c port=usb1
#MCU_ERASE         := $(ST_CMD) -e all
#MCU_READ          := $(ST_CMD) -r
#MCU_WRITE         := $(ST_CMD) -w
#MCU_WRITE_SUFFIX  := .elf
#MCU_VERIFY        := -v

JLINK             := JLinkExe
MCU_ERASE         := $(JLINK) -CommandFile $(MCU_MAKE_DIR)jlink/erase.jlink
MCU_READ          := $(JLINK)
MCU_WRITE         := printf "device STM32F401CC\nsi swd\nspeed 4000\nloadbin
MCU_VERIFY        := ,0x0\nr\ng\nexit" > $(MCU_MAKE_DIR)jlink/flash.jlink; \
                     $(JLINK) -CommandFile $(MCU_MAKE_DIR)jlink/flash.jlink; \
                     rm $(MCU_MAKE_DIR)jlink/flash.jlink
MCU_WRITE_SUFFIX  := .hex

# MCU specific definitions & flags
MCU_DEFS = \
  -DSTM32F401xC \
  -DUSE_HAL_DRIVER \

MCU_C_FLAGS  += \
  -mcpu=cortex-m4 \
  -mthumb \
  -mfloat-abi=hard \
  -mfpu=fpv4-sp-d16

MCU_LD_FLAGS += \
  -mcpu=cortex-m4 \
  -mthumb \
  -mfpu=fpv4-sp-d16 \
  -mfloat-abi=hard \
  -flto \
  -T$(LINKER_SCRIPTS) \
  --specs=nano.specs \
  --specs=nosys.specs

FLAGS = \
  $(MCU_C_FLAGS) \
  -fmessage-length=0 \
  -fsigned-char \
  -ffunction-sections \
  -fdata-sections \
  -ffreestanding \
  -fno-builtin \
  -fno-move-loop-invariants \
  -flto \
  -Wall \
  -Wextra \
  $(MCU_DEFS)

ifeq '$(config)' 'debug'
  FLAGS +=  \
  -Og \
  -ggdb
else
  FLAGS +=  \
  -Ofast \
  -g0
endif

C_FLAGS = \
  $(FLAGS) \
	-std=c99

CPP_FLAGS = \
  $(FLAGS) \
	-std=c++11

$(MCU_LIB): phony
	@echo
	@echo make mcu $(config) library
	@echo
	@make -C $(MCU_MAKE_DIR) -f $(MCU_MAKEFILE) all

$(MAL_LIB): phony
	@echo
	@echo make mal debug library
	@echo
	@make -C $(MAL_MAKE_DIR) -f $(MAL_MAKEFILE) all config=debug
	@echo
	@echo make mal release library
	@echo
	@make -C $(MAL_MAKE_DIR) -f $(MAL_MAKEFILE) all config=release

cleanmal:
	@make -C $(MAL_MAKE_DIR) -f $(MAL_MAKEFILE) clean
	@echo

print-%:
	@echo $* = $($*)
