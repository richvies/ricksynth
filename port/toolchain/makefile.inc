FLASH := st-flash
LINKER_DIR := port
LINKER_SCRIPTS := link.ld
TOOLCHAIN_BIN_PATH := /usr/local/bin
TOOLCHAIN_PREFIX := arm-none-eabi

flash:
	@echo Flashing: $(BUILD_DIR)/$(TARGET_FILE).bin
	$(FLASH) --reset write $(BUILD_DIR)/$(TARGET_FILE).bin $(FLASH_ADDR)

erase:
	@echo Erasing device...
	$(FLASH) erase

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
  -u _printf_float \