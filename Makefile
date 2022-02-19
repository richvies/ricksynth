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

TARGET = $(project)_$(target)_$(config)

SHELL := bash
MKDIR := mkdir -p
RM := rm -rf

BUILD_DIR = build
BIN_DIR   = bin

include port/make/makefile.inc

# Toolchain commands
CC              := '$(TOOLCHAIN_BIN_PATH)/$(TOOLCHAIN_PREFIX)-gcc'
CPP             := '$(TOOLCHAIN_BIN_PATH)/$(TOOLCHAIN_PREFIX)-g++'
OBJDUMP         := '$(TOOLCHAIN_BIN_PATH)/$(TOOLCHAIN_PREFIX)-objdump'
OBJCOPY         := '$(TOOLCHAIN_BIN_PATH)/$(TOOLCHAIN_PREFIX)-objcopy'
SIZE            := '$(TOOLCHAIN_BIN_PATH)/$(TOOLCHAIN_PREFIX)-size'

PYTHON			:= python3

# Source
SRC_DIR += \
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
  voice/processors \

C_FILES ?= $(sort $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.c)))
CPP_FILES ?= $(sort $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.cpp)))
C_OBJS   = $(addprefix $(BUILD_DIR)/, $(notdir $(C_FILES:.c=.o)))
C_OBJS   += $(addprefix $(BUILD_DIR)/, $(notdir $(CPP_FILES:.cpp=.o)))

# So that make does not think they are intermediate files and delete them
.SECONDARY: $(C_OBJS)

VPATH = $(SRC_DIR) $(BUILD_DIR) $(BUILD_DIR) $(BIN_DIR)

INC_PATHS = \
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
  -I./voice/processors

ifeq ("$(V)","1")
  NO_ECHO :=
else
  NO_ECHO := @
endif

# Get the Git commit ID and log
dirty_flag :=
ifneq ("$(shell git diff --shortstat | tail -n1)", "")
  # unstaged changes
  dirty_flag = *
endif
ifneq ("$(shell git diff --cached --shortstat | tail -n1)", "")
  # staged changes
  dirty_flag = *
endif
commit_id := "$(shell git log -1 \
  --pretty=format:'%h$(dirty_flag)')"
commit_log := "$(shell git log -1 --date=short \
  --pretty=format:'[%h$(dirty_flag)] %ad | \"%s\"')"

COMMIT_DEFS = \
  -DCOMMIT_ID='$(commit_id)' \
  -DCOMMIT_LOG='$(commit_log)' \

CFLAGS += $(EXTRA_DEFS)
CFLAGS += $(COMMIT_DEFS)
# create header dependency target file for each object (.d)
CFLAGS += -MD

export CC CFLAGS

#############################################################
# Top builds recipes
#############################################################

.PHONY: all help clean check version flash print- phony

all: $(BIN_DIR) $(BIN_DIR)/$(TARGET).elf $(BIN_DIR)/$(TARGET).hex \
	 $(BIN_DIR)/$(TARGET).bin \
	 $(BIN_DIR)/$(TARGET).lss $(BIN_DIR)/$(TARGET).list
	@echo ''
	@echo 'Size:'
	@echo '-----'
	$(NO_ECHO)$(SIZE) $(BIN_DIR)/$(TARGET).elf
	@echo ''
	@md5sum $(BIN_DIR)/$(TARGET).bin
	$(PYTHON) port/make/finalize.py $(BIN_DIR)/$(TARGET)

$(BIN_DIR):
	$(NO_ECHO)$(MKDIR) -p $(BIN_DIR)

help:
	@echo 'To build the main app for use in a released image:'
	@echo '$$: make clean; make -j8'

clean:
	$(RM) $(BUILD_DIR) $(BIN_DIR)

check:
ifndef TOOLCHAIN_BIN_PATH
	$(error TOOLCHAIN_BIN_PATH is undefined)
endif
ifndef TOOLCHAIN_PREFIX
	$(error TOOLCHAIN_PREFIX is undefined)
endif

version:
	@echo id = $(commit_id)
	@echo log = $(commit_log)

print-%:
	@echo $* = $($*)

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
_EXTRA_ARGS = $(subst \#,\\\#, '$(EXTRA_DEFS) $(LDFLAGS)')
$(eval $(call DEPENDABLE_VAR,_extra))

# bootloader uses git commit info in main.c
ifeq '$(target)' 'boot'
  MAIN_ARGS = $(subst \#,\\\#, '$(COMMIT_DEFS) $(FLASH_DEFS)')
  $(eval $(call DEPENDABLE_VAR,main))
# app uses git commit info info in launch.c
else
  MAIN_ARGS = ''
  LAUNCH_ARGS = $(subst \#,\\\#, '$(COMMIT_DEFS) $(FLASH_DEFS)')
  $(eval $(call DEPENDABLE_VAR,main))
  $(eval $(call DEPENDABLE_VAR,launch))
endif

#############################################################
# File build recipes
#############################################################

%.elf: check $(C_OBJS)
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
	$(NO_ECHO)$(CC) $(CFLAGS) $(C_OBJS) -o $(BIN_DIR)/$(TARGET).elf $(LDFLAGS)

%.bin: %.elf
	@echo Generate: $@
	$(NO_ECHO)$(OBJCOPY) -O binary $(BIN_DIR)/$(TARGET).elf \
  $(BIN_DIR)/$(TARGET).bin

%.hex: %.elf
	@echo Generate: $@
	$(NO_ECHO)$(OBJCOPY) -O ihex $(BIN_DIR)/$(TARGET).elf \
  $(BIN_DIR)/$(TARGET).hex

$(BUILD_DIR)/%.o: %.cpp $(BUILD_DIR)/_extra.arg
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(CPP) -E $(CFLAGS) $(INC_PATHS) -c -o $@.i $<
	$(NO_ECHO)$(CPP) $< -o $@ -c $(CFLAGS) $(INC_PATHS)

$(BUILD_DIR)/%.o: %.c $(BUILD_DIR)/_extra.arg
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(CC) -E $(CFLAGS) $(INC_PATHS) -c -o $@.i $<
	$(NO_ECHO)$(CC) $< -o $@ -c $(CFLAGS) $(INC_PATHS)

%.lss: %.elf
	@echo Generate: $@
	$(NO_ECHO)$(OBJDUMP) -h -S $< > $@

%.list: %.elf
	@echo Generate: $@
	$(NO_ECHO)$(OBJDUMP) -S $< > $@

# include header dependencies
-include $(C_OBJS:.o=.d)

# End of Makefile
