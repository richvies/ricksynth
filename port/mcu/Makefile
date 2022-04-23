include makefile.inc

TARGET          := $(subst .a,,$(notdir $(MCU_LIB)))
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

SOURCE_DIR = \
  src

C_SOURCES = \
  $(sort $(foreach dir, $(SOURCE_DIR), $(wildcard $(dir)/*.c)))

C_OBJS := \
  $(addprefix $(BUILD_DIR), $(C_SOURCES:.c=.o))

C_INCLUDES =  \
$(MCU_INCLUDES) \
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
