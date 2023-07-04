THORSANVIL_ROOT             = $(realpath ../../)

TARGET                      = ThorsStorage.slib

CXXFLAGS					+= -I$(magic_enum_ROOT_DIR)/include
FILE_WARNING_FLAGS          += -Wno-unknown-pragmas
UNITTEST_LINK_LIBS          = ThorsLogging
UNITTEST_LDLIBS             = -ldl

include $(THORSANVIL_ROOT)/build/tools/Makefile
