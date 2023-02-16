STORAGE_MAKE_DIR 		:= $(dir $(lastword $(MAKEFILE_LIST)))

STORAGE_SOURCES = \
  $(STORAGE_MAKE_DIR)/storage.c \
  $(STORAGE_MAKE_DIR)/littlefs/lfs.c \
  $(STORAGE_MAKE_DIR)/littlefs/lfs_util.c

STORAGE_INCLUDES = \
    -I$(STORAGE_MAKE_DIR) \
    -I$(STORAGE_MAKE_DIR)/littlefs
