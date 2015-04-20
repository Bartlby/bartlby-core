# Check if header file exists and add it to the list.
MACRO(CHECK_INCLUDE_FILE_CONCAT FILE VARIABLE)
   CHECK_INCLUDE_FILES("${PROJECT_INCLUDES};${FILE}" ${VARIABLE})
   IF(${VARIABLE})
     SET(PROJECT_INCLUDES ${PROJECT_INCLUDES} ${FILE})
   ENDIF(${VARIABLE})
ENDMACRO(CHECK_INCLUDE_FILE_CONCAT)


MACRO (CHECK_LIBRARY_EXISTS_CONCAT LIBRARY SYMBOL VARIABLE)
  CHECK_LIBRARY_EXISTS ("${LIBRARY};${LINK_LIBS}" ${SYMBOL} "" ${VARIABLE})
  IF (${VARIABLE})
    SET (LINK_LIBS ${LINK_LIBS} ${LIBRARY})
  ENDIF (${VARIABLE})
ENDMACRO (CHECK_LIBRARY_EXISTS_CONCAT)

###CHECK HEADERS
check_include_file_concat("arpa/inet.h"      HAVE_ARPA_INET_H)
check_include_file_concat("sys/prctl.h"	  HAVE_PRCTL_H)
check_include_file_concat("dirent.h"      HAVE_DIRENT_H)
check_include_file_concat("dlfcn.h"          HAVE_DLFCN_H)
check_include_file_concat("inttypes.h"       HAVE_INTTYPES_H)
check_include_file_concat("malloc.h"         HAVE_MALLOC_H)
check_include_file_concat("netdb.h"          HAVE_NETDB_H)
check_include_file_concat("netinet/in.h"     HAVE_NETINET_IN_H)
check_include_file_concat("stdbool.h"        HAVE_STDBOOL_H)
check_include_file_concat("stdint.h"         HAVE_STDINT_H)
check_include_file_concat("stdio.h"          HAVE_STDIO_H)
check_include_file_concat("strings.h"          HAVE_STRINGS_H)
check_include_file_concat("string.h"          HAVE_STRING_H)
check_include_file_concat("syslog.h"          HAVE_SYSLOG_H)

check_include_file_concat("sys/stat.h"       HAVE_SYS_STAT_H)
check_include_file_concat("sys/dir.h"       HAVE_SYS_DIR_H)
check_include_file_concat("sys/types.h"       HAVE_SYS_TYPES_H)
check_include_file_concat("sys/wait.h"       HAVE_SYS_WAIT_H)
check_include_file_concat("vfork.h"       HAVE_VFORK_H)
check_include_file_concat("getopt.h"     HAVE_GETOPT_H)

check_include_file_concat("unistd.h"       HAVE_UNISTD_H)


check_library_exists("dl"   dlopen    ""   HAVE_LIBDL)



check_function_exists(inet_ntoa HAVE_INET_NTOA)


check_symbol_exists(setenv      "stdlib.h"   HAVE_SETENV)
check_symbol_exists(vprintf       "stdio.h"  HAVE_VPRINTF)
check_symbol_exists(vfork        "unistd.h" HAVE_VFORK)
check_symbol_exists(strstr       "string.h"  HAVE_STRSTR)
check_symbol_exists(socket       "sys/socket.h"  HAVE_SOCKET)
check_symbol_exists(strdup       "string.h"  HAVE_STRDUP)
check_symbol_exists(alarm        "unistd.h" HAVE_ALARM)
check_symbol_exists(fork        "unistd.h" HAVE_FORK)
check_symbol_exists(gethostbyname "netdb.h"         HAVE_GETHOSTBYNAME)
check_symbol_exists(gettimeofday "sys/time.h"         HAVE_GETTIMEOFDAY)