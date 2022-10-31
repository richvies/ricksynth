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
# Include mcu specific tools, files & definitions
#############################################################

export config
include board/board.inc
LOCAL_LIBS += $(BRD_LIBS)

#############################################################
# top level source files & includes
#############################################################

SOURCE_DIR = \
  app \
  backbone \
  backbone/math \
  backbone/peripheral \
  backbone/system \
  backbone/util \
  chips/PCF8575 \
  chips/TLC5928 \
  modifiers \
  ui \
  voices \
  voices/generators \
  voice/processors

C_SOURCES = \
  $(sort $(foreach dir, $(SOURCE_DIR), $(wildcard $(dir)/*.c)))

CPP_SOURCES = \
  $(sort $(foreach dir, $(SOURCE_DIR), $(wildcard $(dir)/*.cpp)))

OBJS += $(addprefix $(BUILD_DIR), $(C_SOURCES:.c=.o))
OBJS += $(addprefix $(BUILD_DIR), $(CPP_SOURCES:.cpp=.o))

C_INCLUDES = \
  $(BRD_INCLUDES) \
  -I./ \
  -I./app \
  -I./backbone \
  -I./backbone/math \
  -I./backbone/system \
  -I./backbone/util \
  -I./config \
  -I./modifiers \
  -I./ui \
  -I./voices \
  -I./voices/generators

#############################################################
# flags & definitions
#############################################################

C_DEFS = $(COMMIT_DEFS)

C_FLAGS = \
  $(BRD_C_FLAGS) \
	$(C_DEFS)

CPP_FLAGS = \
  $(BRD_CPP_FLAGS) \
	$(C_DEFS)

LD_FLAGS = \
  $(BRD_LD_FLAGS)

#############################################################
# Top builds recipes
#############################################################

.PHONY: phony

check: phony
ifndef TOOLCHAIN_BIN_PATH
	$(error TOOLCHAIN_BIN_PATH is undefined)
endif
ifndef TOOLCHAIN_PREFIX
	$(error TOOLCHAIN_PREFIX is undefined)
endif

all:  check \
	$(BIN_DIR)$(TARGET).elf \
	$(BIN_DIR)$(TARGET).hex \
	$(BIN_DIR)$(TARGET).bin \
	$(BIN_DIR)$(TARGET).lss \
	$(BIN_DIR)$(TARGET).list
	$(PYTHON) finalize.py $(BIN_DIR)$(TARGET).bin
	@echo

help: phony
	@echo 'To build the main app for use in a released image:'
	@echo '$$: make clean; make -j8'

clean: phony
	$(RM) $(BUILD_DIR) $(BIN_DIR)

cleanall: phony
	@echo
	@echo clean main
	$(RM) $(BUILD_DIR) $(BIN_DIR)
	@echo
	@make cleanboard
	@echo

version: phony
	@echo id = $(commit_id)
	@echo log = $(commit_log)

flash: phony
	@echo Flashing: $(BIN_DIR)$(TARGET)$(MCU_WRITE_SUFFIX)
	$(MCU_WRITE) $(BIN_DIR)$(TARGET)$(MCU_WRITE_SUFFIX) $(MCU_VERIFY)

erase: phony
	@echo Erasing device...
	$(MCU_ERASE)

doc: phony
	doxygen docs/doxyfile

print-%: phony
	@echo $* = $($*)

#############################################################
# File build recipes
#############################################################

$(BIN_DIR)$(TARGET).elf: $(LOCAL_LIBS) $(OBJS) $(BUILD_DIR)link.arg
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
	@echo
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(CPP) $(LD_FLAGS) $(OBJS) -Wl,--start-group $(LOCAL_LIBS) -Wl,--end-group -o $(BIN_DIR)$(TARGET).elf
	$(NO_ECHO) cp $(BIN_DIR)$(TARGET).elf $(BIN_DIR)firmware.elf
	@echo

$(BUILD_DIR)%.o: %.c $(BUILD_DIR)compile.arg
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(CC) -E $(C_FLAGS) $(C_INCLUDES) -c -o $(@:%.o=%.i) $<
	$(NO_ECHO)$(CC) $(C_FLAGS) $(C_INCLUDES) 	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" $< -o $@ -c

$(BUILD_DIR)%.o: %.cpp $(BUILD_DIR)compile.arg
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(MKDIR) -p $(@D)
	$(NO_ECHO)$(CPP) -E $(CPP_FLAGS) $(C_INCLUDES) -c -o $(@:%.o=%.i) $<
	$(NO_ECHO)$(CPP) $(CPP_FLAGS) $(C_INCLUDES) 	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" $< -o $@ -c

# include header dependencies
-include $(OBJS:.o=.d)

%.bin: %.elf
	@echo Generate: $@
	$(NO_ECHO)$(OBJCOPY) -O binary $(BIN_DIR)$(TARGET).elf \
  $(BIN_DIR)$(TARGET).bin

%.hex: %.elf
	@echo Generate: $@
	$(NO_ECHO)$(OBJCOPY) -O ihex $(BIN_DIR)$(TARGET).elf \
  $(BIN_DIR)$(TARGET).hex

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
$(BUILD_DIR)$1.o: $(BUILD_DIR)$1.arg

# get related variable name e.g. main.c -> MAIN_ARGS
$1_arg = $(call $(shell echo '$1' | tr '[:lower:]' '[:upper:]')_ARGS, $1)

# compare and resave if variable != file contents
$(BUILD_DIR)$1.arg: phony
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
COMPILE_ARGS = $(subst \#,\\\#,'$(C_DEFS) $(C_FLAGS)')
$(eval $(call DEPENDABLE_VAR,compile))

LINK_ARGS = $(subst \#,\\\#,'$(LD_FLAGS)')
$(eval $(call DEPENDABLE_VAR,link))

# main uses git commit info
MAIN_ARGS = $(subst \#,\\\#,'$(COMMIT_DEFS)')
$(eval $(call DEPENDABLE_VAR,main))
