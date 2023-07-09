THORSANVIL_ROOT             ?= $(realpath ../../)

TARGET                      = ThorsStorage.slib
NAMESPACE					= ThorsAnvil::FileSystem

MAGIC_ENUM					= $(if $(magicenum_ROOT_DIR), -I$(magicenum_ROOT_DIR)/include)
CXXFLAGS					+= $(MAGIC_ENUM)
UNITTEST_LINK_LIBS          = ThorsLogging
UNITTEST_LDLIBS             = -ldl
VERA						= echo

include $(THORSANVIL_ROOT)/build/tools/Makefile
