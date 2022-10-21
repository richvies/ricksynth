include mcu.inc

#############################################################
# sources + flags
#############################################################

SOURCE_DIR = \
  src

C_SOURCES = \
  $(sort $(foreach dir, $(SOURCE_DIR), $(wildcard $(dir)/*.c)))

C_OBJS := \
  $(subst .o,_$(config).o,$(addprefix $(BUILD_DIR), $(C_SOURCES:.c=.o)))

C_INCLUDES =  \
-Iinclude \
-Isrc \
-I$(MAL_DIR) \
-I$(MAL_DIR)CMSIS \
-I$(MAL_DIR)CMSIS/Include \
-I$(MAL_DIR)USB_DEVICE/App \
-I$(MAL_DIR)USB_DEVICE/Target \
-I$(MAL_DIR)STM32F4xx_HAL_Driver/Inc \
-I$(MAL_DIR)STM32F4xx_HAL_Driver/Inc/Legacy \
-I$(MAL_DIR)Middlewares/ST/STM32_USB_Device_Library/Core/Inc \
-I$(MAL_DIR)Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc

#############################################################
# File build recipes
#############################################################

.PHONY: all clean

all: $(C_OBJS)
	@echo
	@echo archiving mcu $(config) library
	$(NO_ECHO)$(MKDIR) -p $(LIB_DIR)
	$(NO_ECHO)$(AR) rcs $(MCU_LIB) $(C_OBJS)

clean:
	@echo
	@echo clean mcu library
	$(RM) $(BUILD_DIR) $(LIB_DIR)

$(BUILD_DIR)%_$(config).o: %.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(CC) -E $(C_FLAGS) $(C_INCLUDES) -Wno-unused -c -o $(@:%.o=%.i) $<
	$(NO_ECHO)$(CC) $(C_FLAGS) $(C_INCLUDES) -Wno-unused -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" $< -o $@ -c

# include header dependencies
-include $(C_OBJS:.o=.d)
