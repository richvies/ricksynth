TOOLCHAIN_BIN_PATH := /usr/local/bin
TOOLCHAIN_PREFIX := arm-none-eabi
LINKER_SCRIPTS := link.ld
LINKER_DIR := port/toolchain
FLASH := stm32flash -b230400 -v -R -i-dtr,rts,-rts:rts /dev/cu.usbserial-A50285BI
FLASH_ERASE := $(FLASH) -E
FLASH_READ := $(FLASH) -r
FLASH_WRITE := $(FLASH) -w

# flash:
# 	@echo Flashing: $(BUILD_DIR)/$(TARGET_FILE).bin
# 	$(FLASH_WRITE) $(BUILD_DIR)/$(TARGET_FILE).bin

# erase:
# 	@echo Erasing device...
# 	$(FLASH_ERASE)

EXTRA_DEFS = \
  -DSTM32F415xx \
  -DENABLE_IOT_DEBUG \
  -DENABLE_IOT_ERROR \
  -DENABLE_IOT_INFO \
  -DENABLE_IOT_TRACE \
  -DENABLE_IOT_WARN \
  -DUSE_HAL_DRIVER \

CFLAGS = \
  -mcpu=cortex-m4 \
  -mthumb \
  -mfloat-abi=hard \
  -mfpu=fpv4-sp-d16 \
  -O0 \
  -fmessage-length=0 \
  -fsigned-char \
  -ffunction-sections \
  -fdata-sections \
  -ffreestanding \
  -fno-builtin \
  -fno-move-loop-invariants \
  -Wall \
  -Wextra \
  -g3 \

LDFLAGS += \
  -L $(LINKER_DIR) \
  -Xlinker -Map=$(BUILD_DIR)/$(TARGET).map \
  -Xlinker --gc-sections \
  -nostartfiles \
  --specs=nano.specs \
  --specs=nosys.specs \
  -u _printf_float