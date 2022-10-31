# File directories
MCU_MAKE_DIR    := $(dir $(lastword $(MAKEFILE_LIST)))

include $(MCU_MAKE_DIR)mal/mal.inc

MCU_LIB         := $(MCU_MAKE_DIR)$(LIB_DIR)libmcu_$(config).a
MCU_LIBS        := $(MCU_LIB) $(MAL_LIB)

MCU_INCLUDES =  \
  $(MAL_INCLUDES) \
  -I$(MCU_MAKE_DIR)include

MCU_C_FLAGS = \
  $(MAL_C_FLAGS)

MCU_CPP_FLAGS = \
  $(MAL_CPP_FLAGS)

MCU_LD_FLAGS = \
  $(MAL_LD_FLAGS)

.PHONY: phony

$(MCU_LIB): phony
	@make makemcu

makemcu: phony
	@echo
	@echo make mcu $(config) library
	@echo
	@make -C $(MCU_MAKE_DIR) -f mcu.mk all

cleanmcu: phony
	@make -C $(MCU_MAKE_DIR) -f mcu.mk clean
