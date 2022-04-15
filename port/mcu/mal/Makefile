include ../makefile.inc

TARGET          := $(subst .a,,$(notdir $(MAL_LIB)))
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
# sources + flags
#############################################################

C_SOURCES +=  \
USB_DEVICE/App/usb_device.c \
USB_DEVICE/App/usbd_desc.c \
USB_DEVICE/App/usbd_custom_hid_if.c \
USB_DEVICE/Target/usbd_conf.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_adc.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s_ex.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Src/usbd_customhid.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_crc.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_iwdg.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc_ex.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c

C_OBJS := \
  $(addprefix $(BUILD_DIR), $(C_SOURCES:.c=.o))

C_INCLUDES =  \
$(MCU_INCLUDES) \
-I./ \
-ICMSIS \
-ICMSIS/Include \
-IUSB_DEVICE/App \
-IUSB_DEVICE/Target \
-ISTM32F4xx_HAL_Driver/Inc \
-ISTM32F4xx_HAL_Driver/Inc/Legacy \
-IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc \
-IMiddlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc

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

$(LIB_DIR)$(TARGET).a: $(C_OBJS)
	@echo
	@echo archiving $(TARGET)
	$(NO_ECHO)$(MKDIR) -p $(LIB_DIR)
	$(NO_ECHO)$(AR) rcs $(LIB_DIR)$(TARGET).a $(C_OBJS)

$(BUILD_DIR)%.o: %.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(CC) -E $(C_FLAGS) $(C_INCLUDES) -Wno-unused -c -o $(@:%.o=%.i) $<
	$(NO_ECHO)$(CC) $(C_FLAGS) $(C_INCLUDES) -Wno-unused -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" $< -o $@ -c

# include header dependencies
-include $(C_OBJS:.o=.d)
