CHIPS_MAKE_DIR 		:= $(dir $(lastword $(MAKEFILE_LIST)))

CHIPS_SOURCES = \
  $(CHIPS_MAKE_DIR)/w25q/w25q.c \
  $(CHIPS_MAKE_DIR)/pcf8575/pcf8575.c

CHIPS_INCLUDES = \
    -I$(CHIPS_MAKE_DIR) \
