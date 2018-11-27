AC_INIT(crlf, 1.03, bobf@mrp3.com, crlf)
AC_CONFIG_SRCDIR(crlf.c)
AM_INIT_AUTOMAKE
AC_PROG_CC


# recommended by autoscan
AC_CHECK_HEADERS([limits.h fcntl.h])
AC_HEADER_DIRENT
AC_CHECK_FUNCS([munmap getopt])
AC_FUNC_MMAP
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_REPLACE_FNMATCH
AC_FUNC_CLOSEDIR_VOID
AC_CHECK_FUNCS([strchr strrchr])
AC_C_CONST
AC_HEADER_STDC

AC_CANONICAL_HOST

# optional things for Interix vs the rest of the world
case $host in
  i*86-pc-interix*)
    MAN_PAGES="crlf.interix"
    ;;
  *)
    MAN_PAGES="crlf.1"
    ;;
esac

AC_SUBST([MAN_PAGES])


# the end

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

