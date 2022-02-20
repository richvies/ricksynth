# Toolchain commands
TOOLCHAIN_BIN_PATH  := /usr/local/bin
TOOLCHAIN_PREFIX    := arm-none-eabi
CPP                 := '$(TOOLCHAIN_BIN_PATH)/$(TOOLCHAIN_PREFIX)-g++'
CC                  := '$(TOOLCHAIN_BIN_PATH)/$(TOOLCHAIN_PREFIX)-gcc'
AS 							    := '$(TOOLCHAIN_BIN_PATH)/$(TOOLCHAIN_PREFIX)-gcc'
OBJDUMP             := '$(TOOLCHAIN_BIN_PATH)/$(TOOLCHAIN_PREFIX)-objdump'
OBJCOPY             := '$(TOOLCHAIN_BIN_PATH)/$(TOOLCHAIN_PREFIX)-objcopy'
SIZE                := '$(TOOLCHAIN_BIN_PATH)/$(TOOLCHAIN_PREFIX)-size'

# File directories
MAL_DIR         := $(dir $(lastword $(MAKEFILE_LIST)))/mal
LINKER_DIR      := $(dir $(lastword $(MAKEFILE_LIST)))
LINKER_SCRIPTS  := -T $(LINKER_DIR)/link.ld

# Useful commands
FLASH         := stm32flash -b230400 -v -R -i-dtr,rts,-rts:rts /dev/cu.usbserial-A50285BI
FLASH_ERASE   := $(FLASH) -E
FLASH_READ    := $(FLASH) -r
FLASH_WRITE   := $(FLASH) -w

# MCU specific files
ASM_SOURCES +=  \
$(MAL_DIR)/startup_stm32f401xc.s

C_SOURCES   +=  \
$(MAL_DIR)/Core/Src/stm32f4xx_it.c \
$(MAL_DIR)/Core/Src/stm32f4xx_hal_msp.c \
$(MAL_DIR)/USB_DEVICE/App/usb_device.c \
$(MAL_DIR)/USB_DEVICE/App/usbd_desc.c \
$(MAL_DIR)/USB_DEVICE/App/usbd_custom_hid_if.c \
$(MAL_DIR)/USB_DEVICE/Target/usbd_conf.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_adc.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s_ex.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
$(MAL_DIR)/Core/Src/system_stm32f4xx.c \
$(MAL_DIR)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
$(MAL_DIR)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
$(MAL_DIR)/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
$(MAL_DIR)/Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Src/usbd_customhid.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_crc.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_iwdg.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc_ex.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c

C_INCLUDES +=  \
-I./$(MAL_DIR)/USB_DEVICE/App \
-I./$(MAL_DIR)/USB_DEVICE/Target \
-I./$(MAL_DIR)/Core/Inc \
-I./$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Inc \
-I./$(MAL_DIR)/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy \
-I./$(MAL_DIR)/Middlewares/ST/STM32_USB_Device_Library/Core/Inc \
-I./$(MAL_DIR)/Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc \
-I./$(MAL_DIR)/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
-I./$(MAL_DIR)/Drivers/CMSIS/Include

# MCU specific definitions & flags
MCU_DEFS = \
  -DSTM32F401xC \
  -DUSE_HAL_DRIVER \

AS_FLAGS += -x assembler-with-cpp
C_FLAGS  += $(MCU_DEFS)
C_FLAGS  += \
  -mcpu=cortex-m4 \
  -mthumb \
  -mfloat-abi=hard \
  -mfpu=fpv4-sp-d16

LD_FLAGS += \
  -mcpu=cortex-m4 \
  -mthumb \
  -mfpu=fpv4-sp-d16 \
  -mfloat-abi=hard \
  --specs=nano.specs \
  --specs=nosys.specs