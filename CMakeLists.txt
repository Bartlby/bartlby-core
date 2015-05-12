#cmake -DPLUGIN_DIR="/opt/bartlby-agent/plugins/" -DBARTLBY_USER="root" -DMYSQL_HOST="localhost" -DMYSQL_USER=root -DMYSQL_PASS="123" -DMYSQL_DB=bartlby -DCMAKE_INSTALL_PREFIX:PATH=/opt/bartlby -DENABLE_SNMP=1  -DENABLE_LUA=1 -DENABLE_SSH=1 -DFULL_FEATURES=1 ..

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(bartlby-core)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release"
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)


message("CMAKE_C_FLAGS_DEBUG is ${CMAKE_C_FLAGS_DEBUG}")
message("CMAKE_C_FLAGS_RELEASE is ${CMAKE_C_FLAGS_RELEASE}")

message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
IF(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Debug build.")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
  message("Release build.")
ELSE()
  message("Some other build type.")
ENDIF()


if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif()

set(BARTLBY_MAJOR_VERSION 1)
set(BARTLBY_MINOR_VERSION 6)
set(BARTLBY_PATCH_VERSION 5)
set(BARTLBY_VERSION  ${BARTLBY_MAJOR_VERSION}.${BARTLBY_MINOR_VERSION}.${BARTLBY_PATCH_VERSION})

option(FULL_FEATURES, "Enable all features (requires: libssh, libsnmp libssl, libjson" OFF)

if(FULL_FEATURES)
	set(ENABLE_SNMP 1)
	set(ENABLE_SSH 1)
	set(ENABLE_LUA 1)
	set(ENABLE_SSL 1)	
ENDIF()




if(ENABLE_SSL)

	include(cmake/findssl.cmake)
ENDIF()
include(cmake/findmysql.cmake)
include(cmake/findjson.cmake)

include(cmake/findcurl.cmake)

if(ENABLE_LUA)
		include(cmake/findlua.cmake)
ENDIF()

if(ENABLE_SNMP)
	include(cmake/findsnmp.cmake)
ENDIF()

if(ENABLE_SSH)
	include(cmake/findssh.cmake)
ENDIF()

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckFunctionExists)
include(cmake/findgeneric.cmake)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
add_definitions(-DNRPE_ADDON)


if (LIBSSH_INCLUDE_DIRS)
add_definitions(-DSSH_ADDON)
ENDIF()

if(LUA_INCLUDE_DIR)
add_definitions(-DLUA_ADDON)
ENDIF()

if(JSON_INCLUDE_DIR) 
  add_definitions(-DJSON_ADDON)
ENDIF()

if(NETSNMP_INCLUDE_DIR)
	add_definitions(-DSNMP_ADDON)
ENDIF()


if(OPENSSL_FOUND)
add_definitions(-DSSL_ADDON)
ENDIF()


check_library_exists(${JSON_LIBRARY}   json_object_object_get_ex    ""   JSON_OLD)
check_library_exists(${JSON_LIBRARY}   json_object_new_int64    ""   JSON_OLD64)


if(NOT  JSON_OLD)
	add_definitions(-DNEEDS_JSON_GET_EX)
ENDIF()

if(NOT  JSON_OLD64)
	add_definitions(-DNEEDS_JSON_INT64)
ENDIF()


if(NOT MySQL_FOUND OR NOT JSON_INCLUDE_DIR OR NOT CURL_FOUND)
	message( FATAL_ERROR "libjson0/json-c, mysql and CURL are required dependencies" )
ENDIF()

SET(CLIB_DEPS
			src/deps/buffer/clib_buffer.c
			src/deps/progress/progress.c
			
			)
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

SET(TEST_SOURCES
		tests/tinytest.c
		tests/bartlby_test.c
		tests/global.c
		tests/worker.c
		tests/service.c
		tests/server.c
		tests/downtime.c
		tests/traps.c
		tests/servicegroup.c
		tests/servergroup.c
		tests/cleanup.c
		tests/core.c
		tests/lua.c
		tests/triggers.c
		)

set(LIB_LINK  ${LUA_LIBRARY}  ${OPENSSL_LIBRARIES} 	${CURL_LIBRARY}  dl		${JSON_LIBRARY} 		${SSH_LIBRARY}  ${NETSNMP_LIBRARIES}	)


#COMPILE TEST BINARY
set(TEST_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/bartlby.cfg)
add_executable(bartlby_test EXCLUDE_FROM_ALL ${CLIB_DEPS} ${TEST_SOURCES})
SET_TARGET_PROPERTIES(bartlby_test                PROPERTIES COMPILE_FLAGS "-Wno-format-extra-args -Wno-missing-braces -Wno-unused-label -Wno-unused-variable")
SET_TARGET_PROPERTIES(bartlby_test PROPERTIES CMAKE_EXE_LINKER_FLAGS "-Wl,--whole-archive -lbartlby -Wl,--no-whole-archive")
set_target_properties(bartlby_test PROPERTIES COMPILE_DEFINITIONS CONFIG=\"${TEST_CONFIG}\")
target_link_libraries(bartlby_test libbartlby)
target_link_libraries(bartlby_test ${LIB_LINK})


#RUNS TESTS

cmake_policy(PUSH)
if(POLICY CMP0037)
    cmake_policy(SET CMP0037 OLD)
endif()
add_custom_target(test bartlby_test)
cmake_policy(POP)


add_custom_target(update 
				 DEPENDS bartlby bartlby_portier bartlby_portier_legacy libbartlby mysql
				 COMMAND cp -pva bartlby bartlby_portier bartlby_portier_legacy  ${CMAKE_INSTALL_PREFIX}/bin/
				 COMMAND cp -pva mysql.so  ${CMAKE_INSTALL_PREFIX}/lib/

				 )



add_library(libbartlby  ${CLIB_DEPS} ${CORE_SOURCES} )
SET_TARGET_PROPERTIES(libbartlby PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(libbartlby                PROPERTIES COMPILE_FLAGS "-fPIC")
SET_TARGET_PROPERTIES(libbartlby PROPERTIES CMAKE_EXE_LINKER_FLAGS "-Wl,--whole-archive -lbartlby -Wl,--no-whole-archive")
target_link_libraries(libbartlby ${LIB_LINK})


add_executable(bartlby src/bartlby.c)
SET_TARGET_PROPERTIES(bartlby PROPERTIES CMAKE_EXE_LINKER_FLAGS "-Wl,--whole-archive -lbartlby -Wl,--no-whole-archive")
target_link_libraries(bartlby libbartlby)
target_link_libraries(bartlby ${LIB_LINK})



add_executable(bartlby_shmt src/tools/shmt.c)
SET_TARGET_PROPERTIES(bartlby_shmt PROPERTIES CMAKE_EXE_LINKER_FLAGS "-Wl,--whole-archive -lbartlby -Wl,--no-whole-archive")
target_link_libraries(bartlby_shmt libbartlby)
target_link_libraries(bartlby_shmt ${LIB_LINK})

add_library(mysql SHARED ${MYSQL_SOURCES})
SET_TARGET_PROPERTIES(mysql PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(mysql PROPERTIES SUFFIX .so)
SET_TARGET_PROPERTIES(mysql PROPERTIES CMAKE_EXE_LINKER_FLAGS "-fPIC -Wl,--whole-archive -lbartlby -Wl,--no-whole-archive")
target_link_libraries(mysql libbartlby)
target_link_libraries(mysql ${LIB_LINK} ${MySQL_LIBRARY} )



add_executable(bartlby_portier_legacy src/tools/portier_legacy.c)
SET_TARGET_PROPERTIES(bartlby_portier_legacy PROPERTIES CMAKE_EXE_LINKER_FLAGS "-Wl,--whole-archive -lbartlby -Wl,--no-whole-archive")
target_link_libraries(bartlby_portier_legacy libbartlby)
target_link_libraries(bartlby_portier_legacy ${LIB_LINK})



add_executable(bartlby_portier src/tools/portier.c)
SET_TARGET_PROPERTIES(bartlby_portier PROPERTIES CMAKE_EXE_LINKER_FLAGS "-Wl,--whole-archive -lbartlby -Wl,--no-whole-archive")
target_link_libraries(bartlby_portier libbartlby)
target_link_libraries(bartlby_portier ${LIB_LINK})





execute_process( COMMAND openssl dhparam -C 512 
		 COMMAND awk "/^-----/ {exit} {print}"
		 OUTPUT_VARIABLE dha)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/include/bartlby_v2_dh.h ${_dha})



include(cmake/findgit.cmake)

set(prefix ${CMAKE_INSTALL_PREFIX})

configure_file(include/config.h.cmake include/config.h)
configure_file(postinstall-pak.in postinst @ONLY)


INSTALL (SCRIPT cmake/installcfg.cmake ) 

INSTALL(FILES  ${CMAKE_CURRENT_BINARY_DIR}/bartlby.startup 
		 PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE
		 DESTINATION etc/)
INSTALL(FILES mysql.shema DESTINATION ${CMAKE_INSTALL_PREFIX})

install(TARGETS bartlby mysql libbartlby bartlby_portier bartlby_portier_legacy
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

INSTALL(CODE "FILE(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/var/log)") 
INSTALL(CODE "FILE(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/contrib)") 
INSTALL(DIRECTORY perf trigger sbin FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE DESTINATION ${CMAKE_INSTALL_PREFIX})
INSTALL(DIRECTORY  docs DESTINATION ${CMAKE_INSTALL_PREFIX}) 



configure_file(  bartlby.cfg.cmake bartlby.cfg)
configure_file(bartlby.startup.cmake bartlby.startup  @ONLY )

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include/ include/ src/deps/ ${JSON_INCLUDE_DIRS} ${MySQL_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS}   ${LUA_INCLUDE_DIR} ${LIBSSH_INCLUDE_DIRS} ${NETSNMP_INCLUDE_DIR})




message("CONFIG DONE")
message("------------")
get_directory_property( DirDefs DIRECTORY ${CMAKE_SOURCE_DIR} COMPILE_DEFINITIONS )
foreach( d ${DirDefs} )
    message( STATUS "OPTION: " ${d} )
endforeach()



#include(CPack)


#DEBIAN PACKAGE GENERATION
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION_MAJOR ${BARTLBY_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${BARTLBY_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH	"${BARTLBY_PATCH_VERSION}~git-${GIT_VERSION}")
set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_PACKAGE_DESCRIPTION "Bartlby Core")
set(CPACK_PACKAGE_CONTACT "Helmut Januschka <helmut@januschka.com>")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_BINARY_DIR}/postinst")
set(CPACK_SET_DESTDIR "ON")


IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

string(TOLOWER "${CMAKE_PROJECT_NAME}" CPACK_PACKAGE_NAME_LOWERCASE)
find_program(DPKG_PROGRAM dpkg DOC "dpkg program of Debian-based systems")
if(DPKG_PROGRAM)
  execute_process(
    COMMAND ${DPKG_PROGRAM} --print-architecture
    OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  set(CPACK_PACKAGE_FILE_NAME
"${CPACK_PACKAGE_NAME_LOWERCASE}_${BARTLBY_MAJOR_VERSION}.${BARTLBY_MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")   

else(DPKG_PROGRAM)
  set(CPACK_PACKAGE_FILE_NAME
"${CPACK_PACKAGE_NAME_LOWERCASE}_${PROJECT_VERSION}-${PROJECT_VERSION_REVISION}_${CMAKE_SYSTEM_NAME}")
endif(DPKG_PROGRAM)


ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")




include(CPack)



