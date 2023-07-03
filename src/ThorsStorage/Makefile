THORSANVIL_ROOT             = $(realpath ../../)

TARGET                      = ThorsStorage.slib

CXXFLAGS					+= $(INCONSISTENT_MISSING_OVERRIDE) $(DELETE_NON_ABSTRACT_NON_VIRTUAL_DTOR) $(DELETE_NON_VIRTUAL_DTOR)
CXXFLAGS					+= -Wall -Wextra -Werror -Wmissing-braces -Wdeprecated-declarations -Wmissing-field-initializers -Wunused-variable -Wstrict-aliasing -pedantic -Wunreachable-code -Wno-long-long -Wdeprecated
FILE_WARNING_FLAGS          += -Wno-unknown-pragmas
UNITTEST_LINK_LIBS          = ThorsLogging
UNITTEST_LDLIBS             = -ldl

include $(THORSANVIL_ROOT)/build/tools/Makefile
