CONFIG_NAME					= ThorsStorage
THORSANVIL_ROOT				?= $(realpath ../../)
TARGET						= ThorsStorage.slib
NAMESPACE					= ThorsAnvil::FileSystem


LDLIBS_EXTERN_BUILD			+= MagicEnum ThorSerialize
UNITTEST_LDLIBS				+= -ldl

include $(THORSANVIL_ROOT)/build/tools/Makefile
