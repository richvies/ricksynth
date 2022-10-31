include mal.inc

#############################################################
# sources + flags
#############################################################

SOURCE_DIR = \
  $(MAL_MAKE_DIR)USB_DEVICE/App/ \
  $(MAL_MAKE_DIR)USB_DEVICE/Tardget/ \
  $(MAL_MAKE_DIR)STM32F4xx_HAL_Driver/Src/ \
  $(MAL_MAKE_DIR)Middlewares/ST/STM32_USB_Device_Library/Core/Src/ \
  $(MAL_MAKE_DIR)Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Src/ \
  $(MAL_MAKE_DIR)STM32F4xx_HAL_Driver/Src/

C_SOURCES = \
  $(sort $(foreach dir, $(SOURCE_DIR), $(wildcard $(dir)/*.c)))

C_OBJS := \
  $(subst .o,_$(config).o,$(addprefix $(BUILD_DIR), $(C_SOURCES:.c=.o)))

C_INCLUDES =  \
  $(MAL_INCLUDES)

C_FLAGS = \
  $(MAL_C_FLAGS)


#############################################################
# File build recipes
#############################################################

all: $(C_OBJS)
	@echo
	@echo archiving mal $(config) library
	$(NO_ECHO)$(MKDIR) -p $(LIB_DIR)
	$(NO_ECHO)$(AR) rcs $(MAL_LIB) $(C_OBJS)

clean: phony
	@echo
	@echo clean mal library
	$(RM) $(BUILD_DIR) $(LIB_DIR)

$(BUILD_DIR)%_$(config).o: %.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(CC) -E $(C_FLAGS) $(C_INCLUDES) -Wno-unused -c -o $(@:%.o=%.i) $<
	$(NO_ECHO)$(CC) $(C_FLAGS) $(C_INCLUDES) -Wno-unused -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" $< -o $@ -c

# include header dependencies
-include $(C_OBJS:.o=.d)
