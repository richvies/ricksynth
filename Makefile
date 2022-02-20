# ****************************************************************************
#
# RickSynth
# ----------
#
# MIT License
#
# Copyright (c) 2021 Richard Davies
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# ****************************************************************************/

#############################################################
# Project Info
#############################################################

default: all

project ?= synth1
target ?= app
config ?= debug
sampleRate ?= 96000

ifeq '$(project)' 'synth1'
  include app/synth1/synth1.inc
else ifeq '$(project)' 'synth2'
  include app/synth2/synth2.inc
endif

# Get the Git commit ID and log
dirty_flag :=
ifneq ("$(shell git diff --shortstat | tail -n1)", "")
  dirty_flag = +
endif
ifneq ("$(shell git diff --cached --shortstat | tail -n1)", "")
  dirty_flag = +
endif
commit_id := $(shell git log -1 \
  --pretty=format:'%h$(dirty_flag)')
commit_log := "$(shell git log -1 --date=short \
  --pretty=format:'[%h$(dirty_flag)] %ad | \"%s\"')"

COMMIT_DEFS = \
  -DCOMMIT_ID='$(commit_id)' \
  -DCOMMIT_LOG='$(commit_log)'

TARGET = $(project)_$(target)_$(config)_$(commit_id)

#############################################################
# Make tools
#############################################################

ifeq ("$(V)","1")
  NO_ECHO :=
else
  NO_ECHO := @
endif

SHELL 	:= bash
MKDIR 	:= mkdir
RM 			:= rm -rf
PYTHON	:= python3

BUILD_DIR = build
BIN_DIR   = bin

#############################################################
# top level build files & flags
#############################################################

LOCAL_LIBS = port/mcu/lib/libmcu.a
LOCAL_LIBS_DIR = $(addprefix -L, $(dir $(LOCAL_LIBS)))
LIBS = -lc -lm -lnosys -lmcu

SOURCE_DIR += \
  app \
  backbone \
  backbone/math \
  backbone/peripheral \
  backbone/system \
  backbone/util \
  modifiers \
  port \
  port/mcu \
  ui \
  voices \
  voices/generators \
  voice/processors

VPATH = $(SOURCE_DIR) $(BUILD_DIR) $(BIN_DIR)

C_SOURCES   = $(sort $(foreach dir, $(SOURCE_DIR), $(wildcard $(dir)/*.c)))
CPP_SOURCES = $(sort $(foreach dir, $(SOURCE_DIR), $(wildcard $(dir)/*.cpp)))
OBJS    	  = $(addprefix $(BUILD_DIR)/, $(ASM_SOURCES:.s=.o))
OBJS     	 += $(addprefix $(BUILD_DIR)/, $(C_SOURCES:.c=.o))
OBJS    	 += $(addprefix $(BUILD_DIR)/, $(CPP_SOURCES:.cpp=.o))

# So that make does not think they are intermediate files and delete them
.SECONDARY: $(OBJS)

C_INCLUDES = \
  -I./ \
  -I./app \
  -I./backbone \
  -I./backbone/math \
  -I./backbone/system \
  -I./backbone/util \
  -I./config \
  -I./mcu \
  -I./modifiers \
  -I./port \
  -I./port/mcu \
  -I./ui \
  -I./voices \
  -I./voices/generators \

C_FLAGS = \
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
	-std=c99

C_FLAGS += $(COMMIT_DEFS)

LD_FLAGS = \
  -Og \
  -g3 \
  $(LINKER_SCRIPTS) \
  $(LOCAL_LIBS_DIR) \
  $(LIBS) \
  -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref \
  -Wl,--gc-sections \
  -nostartfiles

export CC C_FLAGS


#############################################################
# Include mcu specific tools, files & definitions
#############################################################

include port/mcu/makefile.inc

#############################################################
# Top builds recipes
#############################################################

.PHONY: all mcu help clean check version flash print- phony

check:
ifndef TOOLCHAIN_BIN_PATH
	$(error TOOLCHAIN_BIN_PATH is undefined)
endif
ifndef TOOLCHAIN_PREFIX
	$(error TOOLCHAIN_PREFIX is undefined)
endif

mculib:
	make -C port/mcu -j

port/mcu/lib/libmcu.a: mculib

$(LOCALLIBS):
    $(foreach folder,$(LIBSRC), $(MAKE)-C $(folder) $(MAKECMDGOALS) || exit;)

all:  check \
      $(BIN_DIR) \
		  $(BIN_DIR)/$(TARGET).elf \
		  $(BIN_DIR)/$(TARGET).hex \
	    $(BIN_DIR)/$(TARGET).bin \
	    $(BIN_DIR)/$(TARGET).lss \
			$(BIN_DIR)/$(TARGET).list
	@echo ''
	@echo 'Size:'
	@echo '-----'
	$(NO_ECHO)$(SIZE) $(BIN_DIR)/$(TARGET).elf
	@echo ''
	@md5sum $(BIN_DIR)/$(TARGET).bin
	$(PYTHON) finalize.py $(BIN_DIR)/$(TARGET)

$(BIN_DIR):
	$(NO_ECHO)$(MKDIR) -p $(BIN_DIR)

help:
	@echo 'To build the main app for use in a released image:'
	@echo '$$: make clean; make -j8'

clean:
	$(RM) $(BUILD_DIR) $(BIN_DIR)

version:
	@echo id = $(commit_id)
	@echo log = $(commit_log)

print-%:
	@echo $* = $($*)

flash:
	@echo Flashing: $(BUILD_DIR)/$(TARGET).bin
	$(FLASH_WRITE) $(BUILD_DIR)/$(TARGET).bin

erase:
	@echo Erasing device...
	$(FLASH_ERASE)

#############################################################
# File build recipes
#############################################################

# include header dependencies
-include $(OBJS:.o=.d)

$(BUILD_DIR)/%.o: %.c $(BUILD_DIR)/_extra.arg
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(CC) -E $(C_FLAGS) $(C_INCLUDES) -c -o $(@:%.o=%.i) $<
	$(NO_ECHO)$(CC) $(C_FLAGS) $(C_INCLUDES) 	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" $< -o $@ -c

$(BUILD_DIR)/%.o: %.s $(BUILD_DIR)/_extra.arg
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(AS) $(AS_FLAGS) $(C_FLAGS) $(C_INCLUDES) 	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" $< -o $@ -c

$(BIN_DIR)/$(TARGET).elf: $(OBJS)
	@echo
	@echo 'Make arguments:'
	@echo ----------------
	@echo 'target               = $(TARGET)'
	@echo 'commit_id            = $(commit_id)'
	@echo 'commit_log           = $(commit_log)'
	@echo ----------------
	@echo 'sample rate          = $(sampleRate)'
	@echo ----------------
	@echo
	@echo Linking target: $(TARGET).elf
	$(NO_ECHO)$(CC) $(LD_FLAGS) $(OBJS) -o $(BIN_DIR)/$(TARGET).elf

%.bin: %.elf
	@echo Generate: $@
	$(NO_ECHO)$(OBJCOPY) -O binary $(BIN_DIR)/$(TARGET).elf \
  $(BIN_DIR)/$(TARGET).bin

%.hex: %.elf
	@echo Generate: $@
	$(NO_ECHO)$(OBJCOPY) -O ihex $(BIN_DIR)/$(TARGET).elf \
  $(BIN_DIR)/$(TARGET).hex

%.lss: %.elf
	@echo Generate: $@
	$(NO_ECHO)$(OBJDUMP) -h -S $< > $@

%.list: %.elf
	@echo Generate: $@
	$(NO_ECHO)$(OBJDUMP) -S $< > $@

#############################################################
# Recompile files based on command line argumets
#############################################################

# --- template for checking and saving command-line arguments ---

define DEPENDABLE_VAR

# make obj depend on specified arguments e.g. main.o -> main.arg
$(BUILD_DIR)/$1.o: $(BUILD_DIR)/$1.arg

# get related variable name e.g. main.c -> MAIN_ARGS
$1_arg = $(call $(shell echo '$1' | tr '[:lower:]' '[:upper:]')_ARGS, $1)

# compare and resave if variable != file contents
$(BUILD_DIR)/$1.arg: phony
	$$(NO_ECHO)$$(MKDIR) -p $$(@D)
	@printf "%q\n" '$$($1_arg)' > $$@.tmp; \
	if [[ `diff -N $$@ $$@.tmp` ]]; then \
		printf "%q\n" '$$($1_arg)' > $$@; \
		printf "%b\n" '\n$1.arg changed:\n--------------------\n\
		$$($1_arg)\n' > $$@.msg; \
		cat $$@.msg; \
		rm -rf $$@.msg; \
    printf "%b\n" '------------------------------------\n\n'; \
	fi; \
	rm -rf $$@.tmp
endef
# ---

# create var & recipe for each set of arguments we want to track
# prefix must match name of source file
_EXTRA_ARGS = $(subst \#,\\\#, '$(MCU_DEFS) $(LD_FLAGS)')
$(eval $(call DEPENDABLE_VAR,_extra))

# main uses git commit info
MAIN_ARGS = $(subst \#,\\\#, '$(COMMIT_DEFS)')
$(eval $(call DEPENDABLE_VAR,main))

# End of Makefile
