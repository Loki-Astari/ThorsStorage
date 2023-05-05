THORSANVIL_ROOT             = $(realpath ../../)

TARGET                      = ThorsStorage.slib

CXXFLAGS					+= -I$(magic_enum_ROOT_DIR)/include -Werror -Wmissing-braces -Wdeprecated-declarations -Wmissing-field-initializers -Wunused-variable $(VIRTUAL_WARNING) -Wall -Wextra -Wstrict-aliasing -pedantic -Werror -Wunreachable-code -Wno-long-long -Wdeprecated -Wdeprecated-declarations
FILE_WARNING_FLAGS          += -Wno-unknown-pragmas
UNITTEST_LINK_LIBS          = ThorsLogging
UNITTEST_LDLIBS             = -ldl

include $(THORSANVIL_ROOT)/build/tools/Makefile
