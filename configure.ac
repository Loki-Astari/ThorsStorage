AC_INIT([ThorsStorage], [0.1], [Loki.Astari+ThorsAnvil@gmail.com])
AC_PREREQ([2.65])

AC_CONFIG_MACRO_DIR([build/autotools/m4])
AC_CONFIG_AUX_DIR([build/autotools/build])

AX_THOR_FUNC_INIT_BUILD([ThorsStorage], [src/ThorsStorage/file.tpp], [20])

AX_THOR_FEATURE_HEADER_ONLY_VARIANT([THORS_STORAGE])

AX_THOR_CHECK_USE_MAGIC_ENUM
AX_THOR_CHECK_USE_THORS_SERIALIZE_HEADER_ONLY
AX_THOR_CHECK_USE_THORS_SERIALIZE([ThorSerialize ThorsSocket ThorsStorage])

AX_THOR_DISABLE_TEST_REQUIREING_LOCK_FILES

LT_INIT

AX_THOR_FUNC_TERM_BUILD([STORAGE], [src/ThorsStorage/ThorsStorageConfig.h:config.h.in])

AC_OUTPUT
