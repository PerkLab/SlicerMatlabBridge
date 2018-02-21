#-----------------------------------------------------------------------------
set(MODULE_NAME MatlabCommander)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

#-----------------------------------------------------------------------------
set(MODULE_INCLUDE_DIRECTORIES
  ${OpenIGTLink_INCLUDE_DIRS}
  )

set(MODULE_SRCS
  )

set(MODULE_TARGET_LIBRARIES
  ${ITK_LIBRARIES}
  OpenIGTLink
  vtksys
  )

#-----------------------------------------------------------------------------
SEMMacroBuildCLI(
  NAME ${MODULE_NAME}
  TARGET_LIBRARIES ${MODULE_TARGET_LIBRARIES}
  INCLUDE_DIRECTORIES ${MODULE_INCLUDE_DIRECTORIES}
  ADDITIONAL_SRCS ${MODULE_SRCS}
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${Slicer_CLIMODULES_BIN_DIR}"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${Slicer_CLIMODULES_LIB_DIR}"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${Slicer_CLIMODULES_LIB_DIR}"
  #EXECUTABLE_ONLY
  )

#-----------------------------------------------------------------------------
if(BUILD_TESTING)
  #add_subdirectory(Testing)
endif()

#-----------------------------------------------------------------------------

# Copy (and install) al cli_*.m script files

file(GLOB SCRIPT_FILES 
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/commandserver"
	"${CMAKE_CURRENT_SOURCE_DIR}/commandserver/*.m"
	)

foreach(SCRIPT_FILE ${SCRIPT_FILES})
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/commandserver/${SCRIPT_FILE}
    ${CMAKE_BINARY_DIR}/${Slicer_CLIMODULES_BIN_DIR}/commandserver/${SCRIPT_FILE}    
    COPYONLY)
  install(
    FILES ${CMAKE_BINARY_DIR}/${Slicer_CLIMODULES_BIN_DIR}/commandserver/${SCRIPT_FILE}
    DESTINATION ${Slicer_INSTALL_CLIMODULES_BIN_DIR}/commandserver COMPONENT Runtime)
endforeach()

# Create (and install) version info function

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/commandserver/cli_version.m.in
  ${CMAKE_BINARY_DIR}/${Slicer_CLIMODULES_BIN_DIR}/commandserver/cli_version.m)
install(
  FILES ${CMAKE_BINARY_DIR}/${Slicer_CLIMODULES_BIN_DIR}/commandserver/cli_version.m
  DESTINATION ${Slicer_INSTALL_CLIMODULES_BIN_DIR}/commandserver COMPONENT Runtime)
