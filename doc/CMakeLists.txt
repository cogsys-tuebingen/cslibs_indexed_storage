set(DOXYGEN_DOC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated/)
set(DOXYGEN_FILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYGEN_FILE_OUT ${CMAKE_CURRENT_SOURCE_DIR}/generated/Doxyfile)

configure_file(${DOXYGEN_FILE_IN} ${DOXYGEN_FILE_OUT} @ONLY)

add_custom_target(doc
                  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_FILE_OUT}
                  WORKING_DIRECTORY ${DOXYGEN_DOC_DIR}
                  COMMENT "Generate documentation with doxygen"
                  VERBATIM)
