cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(bartlby)

set(BARTLBY_MAJOR_VERSION 1)
set(BARTLBY_MINOR_VERSION 6)
set(BARTLBY_PATCH_VERSION 5)
set(BARTLBY_VERSION
  ${BARTLBY_MAJOR_VERSION}.${BARTLBY_MINOR_VERSION}.${BARTLBY_PATCH_VERSION})

SET(CLIB_DEPS
			src/deps/buffer/clib_buffer.c
			
			)

	

 set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

add_definitions(-DWITH_NRPE)

SET(CORE_SOURCES
		src/config.c
		src/global.c
		src/sched.c
		src/shm.c
		src/check.c
		src/trigger.c
		src/daemon.c
		src/replication.c
		src/perf.c
		src/downtime.c
		src/ack.c
		src/event.c
		src/check_snmp.c
		src/ext.c
		src/orchestra.c
		src/check_active_v2.c
		src/check_active.c
		src/check_local.c
		src/check_group.c
		src/check_nrpe.c
		src/check_ssh.c
		src/notification_log.c
		src/portier_base.c
		src/compat.c
		src/http.c
		src/check_json.c
		src/webhooks.c
		src/json.c
		src/trap.c
		src/lua.c
		src/trigger_local.c
		src/trigger_webhooks.c
		src/trigger_smtp.c
		src/trigger_lua.c
		)


SET(MYSQL_SOURCES
		src/libs/mysql/traps.c
		src/libs/mysql/worker.c
		src/libs/mysql/server.c
		src/libs/mysql/service.c
		src/libs/mysql/downtime.c
		src/libs/mysql/servergroup.c
		src/libs/mysql/servicegroup.c
		src/libs/mysql/triggers.c
		src/libs/mysql/mysql.c
		)

set(LIB_LINK  ${LUA_LIBRARY}  ${OPENSSL_LIBRARIES} 	${CURL_LIBRARY}  dl		${JSON_LIBRARY} 		${SSH_LIBRARY}	)


add_library(libbartlby  ${CLIB_DEPS} ${CORE_SOURCES} )
SET_TARGET_PROPERTIES(libbartlby PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(libbartlby                PROPERTIES COMPILE_FLAGS "-fPIC")
SET_TARGET_PROPERTIES(libbartlby PROPERTIES CMAKE_EXE_LINKER_FLAGS "-Wl,--whole-archive -lbartlby -Wl,--no-whole-archive")

add_executable(bartlby src/bartlby.c)
target_link_libraries(bartlby libbartlby)
target_link_libraries(bartlby ${LIB_LINK})


add_executable(bartlby_shmt src/tools/shmt.c)
SET_TARGET_PROPERTIES(bartlby_shmt PROPERTIES CMAKE_EXE_LINKER_FLAGS "-Wl,--whole-archive -lbartlby -Wl,--no-whole-archive")
target_link_libraries(bartlby_shmt libbartlby)
target_link_libraries(bartlby_shmt ${LIB_LINK})

add_library(mysql SHARED ${MYSQL_SOURCES})
SET_TARGET_PROPERTIES(mysql PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(mysql PROPERTIES CMAKE_EXE_LINKER_FLAGS "-fPIC -Wl,--whole-archive -lbartlby -Wl,--no-whole-archive")
target_link_libraries(mysql libbartlby)
target_link_libraries(mysql ${LIB_LINK})



add_executable(bartlby_portier_legacy src/tools/portier_legacy.c)
SET_TARGET_PROPERTIES(bartlby_portier_legacy PROPERTIES CMAKE_EXE_LINKER_FLAGS "-Wl,--whole-archive -lbartlby -Wl,--no-whole-archive")
target_link_libraries(bartlby_portier_legacy libbartlby)
target_link_libraries(bartlby_portier_legacy ${LIB_LINK})



add_executable(bartlby_portier src/tools/portier.c)
SET_TARGET_PROPERTIES(bartlby_portier PROPERTIES CMAKE_EXE_LINKER_FLAGS "-Wl,--whole-archive -lbartlby -Wl,--no-whole-archive")
target_link_libraries(bartlby_portier libbartlby)
target_link_libraries(bartlby_portier ${LIB_LINK})


# Search OpenSSL
find_package(PkgConfig REQUIRED)
pkg_search_module(OPENSSL REQUIRED openssl)

if( OPENSSL_FOUND )
	add_definitions(-DHAVE_SSL)
    include_directories(${OPENSSL_INCLUDE_DIRS})
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
else()
    # Error; with REQUIRED, pkg_search_module() will throw an error by it's own
endif()


# - Find mysqlclient
#
# -*- cmake -*-
#
# Find the native MySQL includes and library
#
#  MySQL_INCLUDE_DIR - where to find mysql.h, etc.
#  MySQL_LIBRARIES   - List of libraries when using MySQL.
#  MySQL_FOUND       - True if MySQL found.

IF (MySQL_INCLUDE_DIR AND MySQL_LIBRARY)
  # Already in cache, be silent
  SET(MySQL_FIND_QUIETLY TRUE)
ENDIF (MySQL_INCLUDE_DIR AND MySQL_LIBRARY)

# Include dir
FIND_PATH(MySQL_INCLUDE_DIR 
  NAMES mysql.h
  PATH_SUFFIXES mysql
)

# Library
#SET(MySQL_NAMES mysqlclient mysqlclient_r)
SET(MySQL_NAMES mysqlclient_r)
FIND_LIBRARY(MySQL_LIBRARY
  NAMES ${MySQL_NAMES}
  PATHS /usr/lib /usr/local/lib
  PATH_SUFFIXES mysql
)

IF (MySQL_INCLUDE_DIR AND MySQL_LIBRARY)
  SET(MySQL_FOUND TRUE)
  SET( MySQL_LIBRARIES ${MySQL_LIBRARY} )
ELSE (MySQL_INCLUDE_DIR AND MySQL_LIBRARY)
  SET(MySQL_FOUND FALSE)
  SET( MySQL_LIBRARIES )
ENDIF (MySQL_INCLUDE_DIR AND MySQL_LIBRARY)


IF (MySQL_FOUND)
  add_definitions(-DHAVE_MYSQL)
  IF (NOT MySQL_FIND_QUIETLY)
    MESSAGE(STATUS "Found MySQL: ${MySQL_LIBRARY}")
  ENDIF (NOT MySQL_FIND_QUIETLY)
ELSE (MySQL_FOUND)
  IF (MySQL_FIND_REQUIRED)
    MESSAGE(STATUS "Looked for MySQL libraries named ${MySQL_NAMES}.")
    MESSAGE(FATAL_ERROR "Could NOT find MySQL library")
  ENDIF (MySQL_FIND_REQUIRED)
ENDIF (MySQL_FOUND)

MARK_AS_ADVANCED(
  MySQL_LIBRARY
  MySQL_INCLUDE_DIR
  )



#find JSON-C
if (JSON_INCLUDE_DIR AND JSON_LIBRARY)
  set(JSON_FIND_QUIETLY TRUE)
endif (JSON_INCLUDE_DIR AND JSON_LIBRARY)

find_path(JSON_INCLUDE_DIR json.h
  PATHS /usr/include
  PATH_SUFFIXES json
)

find_library(JSON_LIBRARY
  NAMES json
  PATHS /usr/lib /usr/local/lib
)

set(JSON_LIBRARIES ${JSON_LIBRARY} )


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JSON
  DEFAULT_MSG
  JSON_INCLUDE_DIR
  JSON_LIBRARIES
)

mark_as_advanced(JSON_INCLUDE_DIR JSON_LIBRARY)

if(JSON_INCLUDE_DIR) 
	add_definitions(-DJSON_ADDON)
ENDIF()

###CURL

FIND_PACKAGE(CURL)
IF(CURL_FOUND)
  add_definitions(-DCURL_ADDON)
  MESSAGE(STATUS "Curl libraries found at: ${CURL_LIBRARIES}")
  MESSAGE(STATUS "Curl includes found at: ${CURL_INCLUDE_DIRS}")
else()
  MESSAGE(SEND_ERROR "Could not find cURL on your system")
ENDIF(CURL_FOUND)




##LUA
SET(_POSSIBLE_LUA_INCLUDE include include/lua)
SET(_POSSIBLE_LUA_EXECUTABLE lua)
SET(_POSSIBLE_LUA_LIBRARY lua)

# Determine possible naming suffixes (there is no standard for this)
IF(Lua_FIND_VERSION_MAJOR AND Lua_FIND_VERSION_MINOR)
  SET(_POSSIBLE_SUFFIXES "${Lua_FIND_VERSION_MAJOR}${Lua_FIND_VERSION_MINOR}" "${Lua_FIND_VERSION_MAJOR}.${Lua_FIND_VERSION_MINOR}" "-${Lua_FIND_VERSION_MAJOR}.${Lua_FIND_VERSION_MINOR}")
ELSE(Lua_FIND_VERSION_MAJOR AND Lua_FIND_VERSION_MINOR)
  SET(_POSSIBLE_SUFFIXES "52" "5.2" "-5.2" "51" "5.1" "-5.1")
ENDIF(Lua_FIND_VERSION_MAJOR AND Lua_FIND_VERSION_MINOR)

# Set up possible search names and locations
FOREACH(_SUFFIX ${_POSSIBLE_SUFFIXES})
  LIST(APPEND _POSSIBLE_LUA_INCLUDE "include/lua${_SUFFIX}")
  LIST(APPEND _POSSIBLE_LUA_EXECUTABLE "lua${_SUFFIX}")
  LIST(APPEND _POSSIBLE_LUA_LIBRARY "lua${_SUFFIX}")
ENDFOREACH(_SUFFIX)

# Find the lua executable
FIND_PROGRAM(LUA_EXECUTABLE
  NAMES ${_POSSIBLE_LUA_EXECUTABLE}
)

# Find the lua header
FIND_PATH(LUA_INCLUDE_DIR lua.h
  HINTS
  $ENV{LUA_DIR}
  PATH_SUFFIXES ${_POSSIBLE_LUA_INCLUDE}
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)

# Find the lua library
FIND_LIBRARY(LUA_LIBRARY 
  NAMES ${_POSSIBLE_LUA_LIBRARY}
  HINTS
  $ENV{LUA_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

IF(LUA_LIBRARY)
  # include the math library for Unix
  IF(UNIX AND NOT APPLE)
    FIND_LIBRARY(LUA_MATH_LIBRARY m)
    SET( LUA_LIBRARIES "${LUA_LIBRARY};${LUA_MATH_LIBRARY}" CACHE STRING "Lua Libraries")
  # For Windows and Mac, don't need to explicitly include the math library
  ELSE(UNIX AND NOT APPLE)
    SET( LUA_LIBRARIES "${LUA_LIBRARY}" CACHE STRING "Lua Libraries")
  ENDIF(UNIX AND NOT APPLE)
ENDIF(LUA_LIBRARY)

# Determine Lua version
IF(LUA_INCLUDE_DIR AND EXISTS "${LUA_INCLUDE_DIR}/lua.h")
  FILE(STRINGS "${LUA_INCLUDE_DIR}/lua.h" lua_version_str REGEX "^#define[ \t]+LUA_RELEASE[ \t]+\"Lua .+\"")

  STRING(REGEX REPLACE "^#define[ \t]+LUA_RELEASE[ \t]+\"Lua ([^\"]+)\".*" "\\1" LUA_VERSION_STRING "${lua_version_str}")
  UNSET(lua_version_str)
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LUA_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Lua
                                  REQUIRED_VARS LUA_LIBRARIES LUA_INCLUDE_DIR
                                  VERSION_VAR LUA_VERSION_STRING)

MARK_AS_ADVANCED(LUA_INCLUDE_DIR LUA_LIBRARIES LUA_LIBRARY LUA_MATH_LIBRARY LUA_EXECUTABLE)


if(LUA_INCLUDE_DIR)
add_definitions(-DLUA_ADDON)
ENDIF()



# - Find Net-SNMP
#
# -*- cmake -*-
#
# Find the Net-SNMP module
#
#  NETSNMP_INCLUDE_DIR - where to find Net-SNMP.h, etc.
#  NETSNMP_LIBRARIES   - List of libraries when using Net-SNMP.
#  NETSNMP_FOUND       - True if Net-SNMP found.
 
IF (NETSNMP_INCLUDE_DIR)
  # Already in cache, be silent
  SET(NETSNMP_FIND_QUIETLY TRUE)
ENDIF (NETSNMP_INCLUDE_DIR)
 
FIND_PATH(NETSNMP_INCLUDE_DIR snmp.h
  /usr/include/net-snmp/library
)
 
SET(NETSNMP_NAMES netsnmp)
FIND_LIBRARY(NETSNMP_LIBRARY
  NAMES ${NETSNMP_NAMES}
  PATHS /usr/lib /usr/local/lib
)
 
SET(NETSNMPAGENT_NAMES netsnmpagent)
FIND_LIBRARY(NETSNMPAGENT_LIBRARY
  NAMES ${NETSNMPAGENT_NAMES}
  PATHS /usr/lib /usr/local/lib
)
 
SET(NETSNMPHELPERS_NAMES netsnmphelpers)
FIND_LIBRARY(NETSNMPHELPERS_LIBRARY
  NAMES ${NETSNMPHELPERS_NAMES}
  PATHS /usr/lib /usr/local/lib
)
 
SET(NETSNMPMIBS_NAMES netsnmpmibs)
FIND_LIBRARY(NETSNMPMIBS_LIBRARY
  NAMES ${NETSNMPMIBS_NAMES}
  PATHS /usr/lib /usr/local/lib
)
 
SET(NETSNMPTRAPD_NAMES netsnmptrapd)
FIND_LIBRARY(NETSNMPTRAPD_LIBRARY
  NAMES ${NETSNMPTRAPD_NAMES}
  PATHS /usr/lib /usr/local/lib
)
 
SET(NETSNMP_LIBRARIES
  ${NETSNMP_LIBRARY}
  ${NETSNMPAGENT_LIBRARY}
  ${NETSNMPHELPERS_LIBRARY}
  ${NETSNMPMIBS_LIBRARY}
#  ${NETSNMPTRAPD_LIBRARY}
)
 
 
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NETSNMP
  DEFAULT_MSG
  NETSNMP_INCLUDE_DIR
  NETSNMP_LIBRARIES
)
 
MARK_AS_ADVANCED(
  NETSNMP_LIBRARY
  NETSNMPAGENT_LIBRARY
  NETSNMPHELPERS_LIBRARY
  NETSNMPMIBS_LIBRARY
  NETSNMPTRAPD_LIBRARY
  NETSNMP_INCLUDE_DIR
  )
 

 # - Try to find LibSSH
# Once done this will define
#
#  LIBSSH_FOUND - system has LibSSH
#  LIBSSH_INCLUDE_DIRS - the LibSSH include directory
#  LIBSSH_LIBRARIES - Link these to use LibSSH
#  LIBSSH_DEFINITIONS - Compiler switches required for using LibSSH
#
#  Copyright (c) 2009 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if (LIBSSH_LIBRARIES AND LIBSSH_INCLUDE_DIRS)
  # in cache already
  set(LIBSSH_FOUND TRUE)
else (LIBSSH_LIBRARIES AND LIBSSH_INCLUDE_DIRS)

  find_path(LIBSSH_INCLUDE_DIR
    NAMES
      libssh/libssh.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
      ${CMAKE_INCLUDE_PATH}
      ${CMAKE_INSTALL_PREFIX}/include
  )
  
  find_library(SSH_LIBRARY
    NAMES
      ssh
      libssh
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
      ${CMAKE_LIBRARY_PATH}
      ${CMAKE_INSTALL_PREFIX}/lib
  )

  if (LIBSSH_INCLUDE_DIR AND SSH_LIBRARY)
    set(SSH_FOUND TRUE)
  endif (LIBSSH_INCLUDE_DIR AND SSH_LIBRARY)

  set(LIBSSH_INCLUDE_DIRS
    ${LIBSSH_INCLUDE_DIR}
  )

  if (SSH_FOUND)
    set(LIBSSH_LIBRARIES
      ${LIBSSH_LIBRARIES}
      ${SSH_LIBRARY}
    )

    if (LibSSH_FIND_VERSION)
      file(STRINGS ${LIBSSH_INCLUDE_DIR}/libssh/libssh.h LIBSSH_VERSION_MAJOR
        REGEX "#define[ ]+LIBSSH_VERSION_MAJOR[ ]+[0-9]+")
      # Older versions of libssh like libssh-0.2 have LIBSSH_VERSION but not LIBSSH_VERSION_MAJOR
      if (LIBSSH_VERSION_MAJOR)
        string(REGEX MATCH "[0-9]+" LIBSSH_VERSION_MAJOR ${LIBSSH_VERSION_MAJOR})
	file(STRINGS ${LIBSSH_INCLUDE_DIR}/libssh/libssh.h LIBSSH_VERSION_MINOR
          REGEX "#define[ ]+LIBSSH_VERSION_MINOR[ ]+[0-9]+")
	string(REGEX MATCH "[0-9]+" LIBSSH_VERSION_MINOR ${LIBSSH_VERSION_MINOR})
	file(STRINGS ${LIBSSH_INCLUDE_DIR}/libssh/libssh.h LIBSSH_VERSION_PATCH
          REGEX "#define[ ]+LIBSSH_VERSION_MICRO[ ]+[0-9]+")
	string(REGEX MATCH "[0-9]+" LIBSSH_VERSION_PATCH ${LIBSSH_VERSION_PATCH})

	set(LibSSH_VERSION ${LIBSSH_VERSION_MAJOR}.${LIBSSH_VERSION_MINOR}.${LIBSSH_VERSION_PATCH})

	include(FindPackageVersionCheck)
	find_package_version_check(LibSSH DEFAULT_MSG)
      else (LIBSSH_VERSION_MAJOR)
        message(STATUS "LIBSSH_VERSION_MAJOR not found in ${LIBSSH_INCLUDE_DIR}/libssh/libssh.h, assuming libssh is too old")
        set(LIBSSH_FOUND FALSE)
      endif (LIBSSH_VERSION_MAJOR)
    endif (LibSSH_FIND_VERSION)
  endif (SSH_FOUND)

  # If the version is too old, but libs and includes are set,
  # find_package_handle_standard_args will set LIBSSH_FOUND to TRUE again,
  # so we need this if() here.
  if (LIBSSH_FOUND)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(LibSSH DEFAULT_MSG LIBSSH_LIBRARIES LIBSSH_INCLUDE_DIRS)
  endif (LIBSSH_FOUND)

  # show the LIBSSH_INCLUDE_DIRS and LIBSSH_LIBRARIES variables only in the advanced view
  mark_as_advanced(LIBSSH_INCLUDE_DIRS LIBSSH_LIBRARIES)

endif (LIBSSH_LIBRARIES AND LIBSSH_INCLUDE_DIRS)


if (LIBSSH_INCLUDE_DIRS)
add_definitions(-DSSH_ADDON)
ENDIF()

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckFunctionExists)

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




#find git
find_package(Git)
if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE}  describe --abbrev=4 --always  RESULT_VARIABLE res_var OUTPUT_VARIABLE GIT_COM_ID )
    if( NOT ${res_var} EQUAL 0 )
        set( GIT_VERSION "UNKW")
        message( WARNING "Git failed (not a repo, or no tags). Build will not contain git revision info." )
    endif()
    string( REPLACE "\n" "" GIT_VERSION ${GIT_COM_ID} )
else()
    set( GIT_VERSION  "unknown (git not found!)")
    message( WARNING "Git not found. Build will not contain git revision info." )
endif()

configure_file(include/config.h.cmake include/config.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include/ include/ src/deps/ ${JSON_INCLUDE_DIR} ${MySQL_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS}   ${LUA_INCLUDE_DIR} ${LIBSSH_INCLUDE_DIRS})

