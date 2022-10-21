# BRD Library

# File directories
BRD_MAKE_DIR    := $(dir $(lastword $(MAKEFILE_LIST)))
BRD_LIB         := $(BRD_MAKE_DIR)lib/libbrd_$(config).a
BRD_MAKEFILE    := board.mk

include $(BRD_MAKE_DIR)mcu/mcu.inc

BRD_LIBS				:= $(BRD_LIB) $(MCU_LIB) $(MAL_LIB)

$(BRD_LIB): phony $(MCU_LIB) $(MAL_LIB)
	@echo
	@echo make brd library $(config)
	@make -C $(BRD_MAKE_DIR) -f $(BRD_MAKEFILE) all

print-%:
	@echo $* = $($*)
