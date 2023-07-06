THORSANVIL_ROOT             ?= $(realpath ../../)

TARGET                      = ThorsStorage.slib
NAMESPACE					= ThorsAnvil::FileSystem

CXXFLAGS					+= -I$(magic_enum_ROOT_DIR)/include
UNITTEST_LINK_LIBS          = ThorsLogging
UNITTEST_LDLIBS             = -ldl
VERA						= echo

include $(THORSANVIL_ROOT)/build/tools/Makefile
