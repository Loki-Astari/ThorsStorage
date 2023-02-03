THORSANVIL_ROOT             = $(realpath ../../)

TARGET                      = ThorsStorage.slib

CXXFLAGS					+= -Werror -Wmissing-braces -Wdeprecated-declarations -Wmissing-field-initializers -Wunused-variable -Winconsistent-missing-override -Wdelete-non-abstract-non-virtual-dtor -Wall -Wextra -Wstrict-aliasing -pedantic -Werror -Wunreachable-code -Wno-long-long -Wdeprecated -Wdeprecated-declarations
FILE_WARNING_FLAGS          += -Wno-unknown-pragmas
UNITTEST_LINK_LIBS          = ThorsLogging
UNITTEST_LDLIBS             = -ldl

include $(THORSANVIL_ROOT)/build/tools/Makefile
