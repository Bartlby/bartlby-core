#cmake -DPLUGIN_DIR="/opt/bartlby-agent/plugins/" -DBARTLBY_USER="root" -DMYSQL_HOST="localhost" -DMYSQL_USER=root -DMYSQL_PASS="123" -DMYSQL_DB=bartlby -DCMAKE_INSTALL_PREFIX:PATH=/opt/bartlby ..

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(bartlby)

include(cmake/findssl.cmake)
include(cmake/findmysql.cmake)
include(cmake/findjson.cmake)
include(cmake/findcurl.cmake)
include(cmake/findlua.cmake)
include(cmake/findsnmp.cmake)
include(cmake/findssh.cmake)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckFunctionExists)
include(cmake/findgeneric.cmake)


set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
add_definitions(-DWITH_NRPE)


if (LIBSSH_INCLUDE_DIRS)
add_definitions(-DSSH_ADDON)
ENDIF()
if(LUA_INCLUDE_DIR)
add_definitions(-DLUA_ADDON)
ENDIF()

if(JSON_INCLUDE_DIR) 
  add_definitions(-DJSON_ADDON)
ENDIF()

if(OPENSSL_FOUND)
add_definitions(-DHAVE_SSL)
ENDIF()



if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif()

set(BARTLBY_MAJOR_VERSION 1)
set(BARTLBY_MINOR_VERSION 6)
set(BARTLBY_PATCH_VERSION 5)
set(BARTLBY_VERSION
  ${BARTLBY_MAJOR_VERSION}.${BARTLBY_MINOR_VERSION}.${BARTLBY_PATCH_VERSION})

SET(CLIB_DEPS
			src/deps/buffer/clib_buffer.c
			
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

set(LIB_LINK  ${LUA_LIBRARY}  ${OPENSSL_LIBRARIES} 	${CURL_LIBRARY}  dl		${JSON_LIBRARY} 		${SSH_LIBRARY}  	)


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

configure_file(include/config.h.cmake include/config.h)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/bartlby.cfg DESTINATION etc/)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/bartlby.startup DESTINATION etc/)
INSTALL(FILES mysql.shema DESTINATION ${CMAKE_INSTALL_PREFIX})

install(TARGETS bartlby mysql libbartlby bartlby_portier bartlby_portier_legacy
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

INSTALL(CODE "FILE(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/var/log)") 
INSTALL(CODE "FILE(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/contrib)") 
INSTALL(DIRECTORY perf trigger DESTINATION ${CMAKE_INSTALL_PREFIX}) 


set(prefix ${CMAKE_INSTALL_PREFIX})
configure_file(  bartlby.cfg.cmake bartlby.cfg)
configure_file(bartlby.startup.cmake bartlby.startup @ONLY )

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include/ include/ src/deps/ ${JSON_INCLUDE_DIRS} ${MySQL_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS}   ${LUA_INCLUDE_DIR} ${LIBSSH_INCLUDE_DIRS})

