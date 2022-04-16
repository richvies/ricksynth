# MCU Library

# File directories
MCU_MAKE_DIR    := $(dir $(lastword $(MAKEFILE_LIST)))
MCU_LIB         := $(MCU_MAKE_DIR)lib/libmcu.a

MAL_DIR         := $(MCU_MAKE_DIR)mal/
MAL_MAKE_DIR    := $(MAL_DIR)
MAL_LIB         := $(MAL_MAKE_DIR)lib/libmal.a 
LINKER_SCRIPTS  := $(MAL_MAKE_DIR)link.ld

# Toolchain commands
TOOLCHAIN_BIN_PATH  := /usr/local/bin/
TOOLCHAIN_PREFIX    := arm-none-eabi
CPP                 := '$(TOOLCHAIN_BIN_PATH)$(TOOLCHAIN_PREFIX)-g++'
CC                  := '$(TOOLCHAIN_BIN_PATH)$(TOOLCHAIN_PREFIX)-gcc'
AS 							    := '$(TOOLCHAIN_BIN_PATH)$(TOOLCHAIN_PREFIX)-gcc'
AR 							    := '$(TOOLCHAIN_BIN_PATH)$(TOOLCHAIN_PREFIX)-ar'
OBJDUMP             := '$(TOOLCHAIN_BIN_PATH)$(TOOLCHAIN_PREFIX)-objdump'
OBJCOPY             := '$(TOOLCHAIN_BIN_PATH)$(TOOLCHAIN_PREFIX)-objcopy'
SIZE                := '$(TOOLCHAIN_BIN_PATH)$(TOOLCHAIN_PREFIX)-size'

# Useful commands
ST_CMD            := STM32_Programmer_CLI -c port=usb1
MCU_ERASE         := $(ST_CMD) -e all
MCU_READ          := $(ST_CMD) -r
MCU_WRITE         := $(ST_CMD) -w
MCU_WRITE_SUFFIX  := .elf
MCU_VERIFY        := -v

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
  -T$(LINKER_SCRIPTS) \
  --specs=nano.specs \
  --specs=nosys.specs
