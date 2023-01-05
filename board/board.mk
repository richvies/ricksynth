include board.inc
include chips/chips.inc

#############################################################
# sources + flags
#############################################################

SOURCE_DIR = \
  . \
	chips \
	storage \
	storage/littlefs \

C_SOURCES = \
  $(sort $(foreach dir, $(SOURCE_DIR), $(wildcard $(dir)/*.c)))

C_OBJS := \
  $(subst .o,_$(config).o,$(addprefix $(BUILD_DIR), $(C_SOURCES:.c=.o)))

C_INCLUDES =  \
  $(BRD_INCLUDES) \
  $(MCU_INCLUDES) \
  -Iconfig \
  -Ichips \
  -Istorage \
  -Istorage/littlefs \

C_FLAGS = \
  $(MCU_C_FLAGS)


#############################################################
# File build recipes
#############################################################

all: $(C_OBJS)
	@echo
	@echo archiving brd $(config) library
	$(NO_ECHO)$(MKDIR) -p $(LIB_DIR)
	$(NO_ECHO)$(AR) rcs $(BRD_LIB) $(C_OBJS)

clean:
	@echo
	@echo clean brd library
	$(RM) $(BUILD_DIR) $(LIB_DIR)

$(BUILD_DIR)%_$(config).o: %.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(CC) -E $(C_FLAGS) $(C_INCLUDES) -c -o $(@:%.o=%.i) $<
	$(NO_ECHO)$(CC) $(C_FLAGS) $(C_INCLUDES) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" $< -o $@ -c

# include header dependencies
-include $(C_OBJS:.o=.d)
