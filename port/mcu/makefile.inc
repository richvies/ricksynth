# MCU Library
MCU_MAKE_DIR    := $(dir $(lastword $(MAKEFILE_LIST)))
MCU_LIB         := $(dir $(lastword $(MAKEFILE_LIST)))lib/libmcu.a

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

# File directories
MAL_DIR         := $(dir $(lastword $(MAKEFILE_LIST)))mal/
LINKER_SCRIPTS  := -T $(MAL_DIR)STM32F401CCUx_FLASH.ld

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

MCU_AS_FLAGS += \
  -x assembler-with-cpp

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
  $(LINKER_SCRIPTS) \
  --specs=nano.specs \
  --specs=nosys.specs