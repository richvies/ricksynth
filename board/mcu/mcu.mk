include mcu.inc

#############################################################
# sources + flags
#############################################################

SOURCE_DIR = \
  $(MCU_MAKE_DIR)src

C_SOURCES = \
  $(sort $(foreach dir, $(SOURCE_DIR), $(wildcard $(dir)/*.c)))

C_OBJS := \
  $(subst .o,_$(config).o,$(addprefix $(BUILD_DIR), $(C_SOURCES:.c=.o)))


C_INCLUDES =  \
  $(MCU_INCLUDES) \
  $(MAL_INCLUDES) \
  -I$(MCU_MAKE_DIR)src \
	-I$(MCU_MAKE_DIR)../config

C_FLAGS = \
  $(MAL_C_FLAGS) \
  $(MCU_C_FLAGS)

#############################################################
# File build recipes
#############################################################

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

$(BUILD_DIR)config/config_mcu_$(config).o: $(MCU_MAKE_DIR)../config/config_mcu.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(CC) -E $(C_FLAGS) $(C_INCLUDES) -Wno-unused -c -o $(@:%.o=%.i) $<
	$(NO_ECHO)$(CC) $(C_FLAGS) $(C_INCLUDES) -Wno-unused -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" $< -o $@ -c

# include header dependencies
-include $(C_OBJS:.o=.d)
