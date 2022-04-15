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
# sources + flags
#############################################################

C_SOURCES = \
$(MCU_BOARD) \
src/_hw.c \
src/_vectors.c \
src/clk.c \
src/io.c \
src/i2c.c \
src/irq.c

C_SOURCES +=  \
$(MAL_DIR)USB_DEVICE/App/usb_device.c \
$(MAL_DIR)USB_DEVICE/App/usbd_desc.c \
$(MAL_DIR)USB_DEVICE/App/usbd_custom_hid_if.c \
$(MAL_DIR)USB_DEVICE/Target/usbd_conf.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_adc.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s_ex.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
$(MAL_DIR)Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
$(MAL_DIR)Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
$(MAL_DIR)Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
$(MAL_DIR)Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Src/usbd_customhid.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_crc.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_iwdg.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rtc_ex.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
$(MAL_DIR)STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c

C_OBJS := \
  $(addprefix $(BUILD_DIR), $(C_SOURCES:.c=.o))

C_INCLUDES =  \
$(MCU_INCLUDES) \
-I./ \
-I$(MAL_DIR) \
-I$(MAL_DIR)CMSIS \
-I$(MAL_DIR)CMSIS/Include \
-I$(MAL_DIR)USB_DEVICE/App \
-I$(MAL_DIR)USB_DEVICE/Target \
-I$(MAL_DIR)STM32F4xx_HAL_Driver/Inc \
-I$(MAL_DIR)STM32F4xx_HAL_Driver/Inc/Legacy \
-I$(MAL_DIR)Middlewares/ST/STM32_USB_Device_Library/Core/Inc \
-I$(MAL_DIR)Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc

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
