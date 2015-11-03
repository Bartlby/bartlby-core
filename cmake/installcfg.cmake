MACRO (COPY_IF_NOT_EXISTS _SRC _DEST) 
        MESSAGE ("Copy ${CMAKE_CURRENT_SOURCE_DIR}/${_SRC} to 
${CMAKE_INSTALL_PREFIX}/${_DEST}") 
        IF (NOT EXISTS ${CMAKE_INSTALL_PREFIX}/${_DEST}) 
                EXECUTE_PROCESS (COMMAND ${CMAKE_COMMAND} -E copy 
${CMAKE_CURRENT_SOURCE_DIR}/${_SRC} 
                                                          
${CMAKE_INSTALL_PREFIX}/${_DEST}) 
        ENDIF (NOT EXISTS ${CMAKE_INSTALL_PREFIX}/${_DEST}) 
ENDMACRO (COPY_IF_NOT_EXISTS _SRC _DEST) 

COPY_IF_NOT_EXISTS ("bartlby.cfg" "etc/bartlby.cfg") 