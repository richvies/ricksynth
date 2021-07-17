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

include port/toolchain/makefile.inc

project ?= synth1
target ?= app
config ?= debug
sampleRate ?= 96000

ifeq '$(project)' 'synth1'
  include app/synth1/synth1.inc
else ifeq '$(project)' 'synth2'
  include app/synth2/synth2.inc
endif

TARGET_FILE = $(project)_$(target)_$(config)

SHELL := bash
MKDIR := mkdir -p
RM := rm -rf

BUILD_DIR = build
BIN_DIR   = $(BUILD_DIR)
OBJ_DIR   = $(BUILD_DIR)/obj

# Toolchain commands
CC              := '$(TOOLCHAIN_BIN_PATH)/$(TOOLCHAIN_PREFIX)-gcc'
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
  port/backbone \
  test \
  ui \
  voices \
  voices/generators \
  voice/processors \

C_FILES ?= $(sort $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.c)))
C_OBJS   = $(addprefix $(OBJ_DIR)/, $(notdir $(C_FILES:.c=.o)))

# So that make does not think they are intermediate files and delete them
.SECONDARY: $(C_OBJS)

VPATH = $(SRC_DIR) $(OBJ_DIR) $(BUILD_DIR) $(BIN_DIR)

INC_PATHS = -I./

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

.PHONY: all help clean check version flash print-

all: $(BIN_DIR)/$(TARGET_FILE).elf $(BIN_DIR)/$(TARGET_FILE).hex \
	 $(BIN_DIR)/$(TARGET_FILE).bin \
	 $(BIN_DIR)/$(TARGET_FILE).lss $(BIN_DIR)/$(TARGET_FILE).list
	@echo ''
	@echo 'Size:'
	@echo '-----'
	$(NO_ECHO)$(SIZE) $(BUILD_DIR)/$(TARGET_FILE).elf
	@echo ''
	@md5sum $(BUILD_DIR)/$(TARGET_FILE).bin
	$(PYTHON) finalize.py $(BUILD_DIR)/$(TARGET_FILE)

help:
	@echo 'To build the main app for use in a released image:'
	@echo '$$: make clean; make -j8'

clean:
	$(RM) $(BUILD_DIR)

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
$(BUILD_DIR)/$1.arg: $(BUILD_DIR) phony
	@printf "%q\n" '$$($1_arg)' > $$@.tmp; \
	if [[ `diff -N $$@ $$@.tmp` ]]; then \
		printf "%q\n" '$$($1_arg)' > $$@; \
		printf "%b\n" '\n$1.arg changed:\n--------------------\n\
		$$($1_arg)\n' > $$@.msg; \
		cat $$@.msg; \
		rm -rf $$@.msg; \
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
	@echo 'target               = $(TARGET_FILE)'
	@echo 'commit_id            = $(commit_id)'
	@echo 'commit_log           = $(commit_log)'
	@echo ----------------
	@echo 'sample rate          = $(sampleRate)'
	@echo ----------------
	@echo
	@echo Linking target: $(TARGET_FILE).elf
	$(NO_ECHO)$(CC) $(CFLAGS) $(C_OBJS) -o $(BUILD_DIR)/$(TARGET_FILE).elf \
	$(LDFLAGS)

%.bin: %.elf
	@echo Generate: $@
	$(NO_ECHO)$(OBJCOPY) -O binary $(BUILD_DIR)/$(TARGET_FILE).elf \
	$(BUILD_DIR)/$(TARGET_FILE).bin

%.hex: %.elf
	@echo Generate: $@
	$(NO_ECHO)$(OBJCOPY) -O ihex $(BUILD_DIR)/$(TARGET_FILE).elf \
	$(BUILD_DIR)/$(TARGET_FILE).hex

$(OBJ_DIR)/%.o: %.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(CC) -E $(CFLAGS) $(LDFLAGS) $(INC_PATHS) -c -o $@.i $<
	$(NO_ECHO)$(CC) $(CFLAGS) $(LDFLAGS) $(INC_PATHS) -c -o $@ $<

%.lss: %.elf
	@echo Generate: $@
	$(NO_ECHO)$(OBJDUMP) -h -S $< > $@

%.list: %.elf
	@echo Generate: $@
	$(NO_ECHO)$(OBJDUMP) -S $< > $@

# include header dependencies
-include $(C_OBJS:.o=.d)

# End of Makefile
