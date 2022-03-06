include makefile.inc

TARGET          := $(subst .a,,$(notdir $(MCU_LIB)))
MAL_DIR         := mal/
BUILD_DIR       := build/
LIB_DIR         := lib/

MKDIR           := mkdir
RM 			        := rm -rf
ifeq ("$(V)","1")
  NO_ECHO :=
else
  NO_ECHO := @
endif

#############################################################
# STM32CubeMx sources
#############################################################

# C sources
C_SOURCES =  \
Core/Src/stm32f4xx_it.c \
Core/Src/stm32f4xx_hal_msp.c \
USB_DEVICE/App/usb_device.c \
USB_DEVICE/App/usbd_desc.c \
USB_DEVICE/App/usbd_custom_hid_if.c \
USB_DEVICE/Target/usbd_conf.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_adc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
Core/Src/system_stm32f4xx.c \
Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Src/usbd_customhid.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_crc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_iwdg.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c

# ASM sources
ASM_SOURCES =  \
startup_stm32f401xc.s

# C includes
C_INCLUDES =  \
-IUSB_DEVICE/App \
-IUSB_DEVICE/Target \
-ICore/Inc \
-IDrivers/STM32F4xx_HAL_Driver/Inc \
-IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy \
-IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc \
-IMiddlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc \
-IDrivers/CMSIS/Device/ST/STM32F4xx/Include \
-IDrivers/CMSIS/Include

# add directory as prefix makes it easy to sources straight from stm32 Makefile
ASM_SOURCES := $(addprefix $(MAL_DIR), $(ASM_SOURCES))
C_SOURCES   := $(addprefix $(MAL_DIR), $(C_SOURCES))
C_INCLUDES  := $(subst -I,-I$(MAL_DIR), $(C_INCLUDES))

OBJS := \
  $(addprefix $(BUILD_DIR), $(ASM_SOURCES:.s=.o)) \
  $(addprefix $(BUILD_DIR), $(C_SOURCES:.c=.o))

# MCU specific definitions & flags
AS_FLAGS += \
  $(MCU_AS_FLAGS)

C_FLAGS = \
  $(MCU_C_FLAGS) \
  -Og \
  -g3 \
  -fmessage-length=0 \
  -fsigned-char \
  -ffunction-sections \
  -fdata-sections \
  -ffreestanding \
  -fno-builtin \
  -fno-move-loop-invariants \
  -w \
  -Wall \
  -Wextra \
	-std=c99 \
  $(MCU_DEFS)

#############################################################
# File build recipes
#############################################################

.PHONY: all clean

all: $(LIB_DIR)$(TARGET).a

clean:
	$(RM) $(BUILD_DIR) $(LIB_DIR)

$(LIB_DIR)$(TARGET).a: $(OBJS)
	@echo
	@echo archiving $(TARGET)
	$(NO_ECHO)$(MKDIR) -p $(LIB_DIR)
	$(NO_ECHO)$(AR) rcs $(LD_FLAGS) $(LIB_DIR)$(TARGET).a $(OBJS)

$(BUILD_DIR)%.o: %.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(CC) -E $(C_FLAGS) $(C_INCLUDES) -c -o $(@:%.o=%.i) $<
	$(NO_ECHO)$(CC) $(C_FLAGS) $(C_INCLUDES) 	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" $< -o $@ -c

$(BUILD_DIR)%.o: %.s
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(AS) $(AS_FLAGS) $(C_FLAGS) $(C_INCLUDES) 	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" $< -o $@ -c

# include header dependencies
-include $(OBJS:.o=.d)
