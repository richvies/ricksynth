# BRD Library

# File directories
BRD_MAKE_DIR    := $(dir $(lastword $(MAKEFILE_LIST)))

include $(BRD_MAKE_DIR)mcu/mcu.inc

BRD_LIB         := $(BRD_MAKE_DIR)$(LIB_DIR)libbrd_$(config).a
BRD_MAKEFILE    := board.mk

BRD_LIBS				:= $(BRD_LIB) $(MCU_LIB) $(MAL_LIB)

$(BRD_LIB): phony $(MCU_LIB) $(MAL_LIB)
	@echo
	@echo make brd $(config) library
	@echo
	@make -C $(BRD_MAKE_DIR) -f $(BRD_MAKEFILE) all

print-%:
	@echo $* = $($*)
