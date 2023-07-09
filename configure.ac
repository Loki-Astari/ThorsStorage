AC_INIT([ThorsStorage], [0.1], [Loki.Astari+ThorsAnvil@gmail.com])
AC_PREREQ([2.65])

AC_CONFIG_MACRO_DIR([build/autotools/m4])
AC_CONFIG_AUX_DIR([build/autotools/build])

AX_THOR_FUNC_INIT_BUILD([ThorsStorage], [17], [src/ThorsStorage/file.tpp])


AX_THOR_CHECK_USE_MAGIC_ENUM
AX_THOR_CHECK_USE_THORS_SERIALIZE

LT_INIT

AX_THOR_FUNC_TERM_BUILD([STORAGE], [src/ThorsStorage/ThorsStorageConfig.h])

AC_OUTPUT
