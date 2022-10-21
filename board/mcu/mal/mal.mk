include ../mcu.inc

#############################################################
# sources + flags
#############################################################

SOURCE_DIR = \
USB_DEVICE/App/ \
USB_DEVICE/Target/ \
STM32F4xx_HAL_Driver/Src/ \
Middlewares/ST/STM32_USB_Device_Library/Core/Src/ \
Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Src/ \
STM32F4xx_HAL_Driver/Src/

C_SOURCES = \
  $(sort $(foreach dir, $(SOURCE_DIR), $(wildcard $(dir)/*.c)))

C_OBJS := \
  $(addprefix $(BUILD_DIR), $(C_SOURCES:.c=.o))

C_INCLUDES =  \
-I./ \
-ICMSIS \
-ICMSIS/Include \
-IUSB_DEVICE/App \
-IUSB_DEVICE/Target \
-ISTM32F4xx_HAL_Driver/Inc \
-ISTM32F4xx_HAL_Driver/Inc/Legacy \
-IMiddlewares/ST/STM32_USB_Device_Library/Core/Inc \
-IMiddlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc

#############################################################
# File build recipes
#############################################################

.PHONY: all clean

all: $(C_OBJS)
	@echo
	@echo archiving mal library
	$(NO_ECHO)$(MKDIR) -p $(LIB_DIR)
	$(NO_ECHO)$(AR) rcs $(MAL_LIB) $(C_OBJS)

clean:
	@echo
	@echo clean mal library
	$(RM) $(BUILD_DIR) $(LIB_DIR)

$(BUILD_DIR)%.o: %.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(CC) -E $(C_FLAGS) $(C_INCLUDES) -Wno-unused -c -o $(@:%.o=%.i) $<
	$(NO_ECHO)$(CC) $(C_FLAGS) $(C_INCLUDES) -Wno-unused -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" $< -o $@ -c

# include header dependencies
-include $(C_OBJS:.o=.d)
