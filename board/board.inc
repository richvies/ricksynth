# BRD Library

BRD_MAKE_DIR 		:= $(dir $(lastword $(MAKEFILE_LIST)))

include $(BRD_MAKE_DIR)mcu/mcu.inc

BRD_LIB         := $(BRD_MAKE_DIR)$(LIB_DIR)libbrd_$(config).a
BRD_LIBS       	:= $(BRD_LIB) $(MCU_LIBS)

BRD_INCLUDES =  \
  -I$(BRD_MAKE_DIR)

BRD_C_FLAGS = \
  $(MCU_C_FLAGS)

BRD_CPP_FLAGS = \
  $(MCU_CPP_FLAGS)

BRD_LD_FLAGS = \
  $(MCU_LD_FLAGS)

$(BRD_LIB): makeboard

makeboard: phony
	@echo
	@echo make brd $(config) library
	@echo
	@make -C $(BRD_MAKE_DIR) -f board.mk all

cleanboard: phony
	@make -C $(BRD_MAKE_DIR) -f board.mk clean
	@make cleanmcu
