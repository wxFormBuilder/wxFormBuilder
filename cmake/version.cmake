cmake_minimum_required(VERSION 3.20)

#[[
Insert application version into a source code file.

get_version(<target>
            INPUT_FILE <input-file>
            OUTPUT_FILE <output-file>)

In the template file <input-file> the placeholders get replaced with version information and the result
is written to <output-file>. If <input-file> is not absolute it is interpreted relative to ${CMAKE_CURRENT_SOURCE_DIR},
if <output-file> is not absolute it is interpreted relative to ${CMAKE_CURRENT_BINARY_DIR}.
The <input-file>, <output-file> get added as source to <target>.

Version specific template parameters:
- wxFormBuilder_VERSION
]]
function(get_version arg_TARGET)
  set(options "")
  set(singleValues INPUT_FILE OUTPUT_FILE)
  set(multiValues "")
  cmake_parse_arguments(arg "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  if(NOT DEFINED arg_INPUT_FILE)
    message(FATAL_ERROR "No input file specified")
  else()
    cmake_path(ABSOLUTE_PATH arg_INPUT_FILE BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE)
  endif()
  if(NOT DEFINED arg_OUTPUT_FILE)
    message(FATAL_ERROR "No output file specified")
  else()
    cmake_path(ABSOLUTE_PATH arg_OUTPUT_FILE BASE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" NORMALIZE)
  endif()

  configure_file("${arg_INPUT_FILE}" "${arg_OUTPUT_FILE}" @ONLY)
  target_sources(${arg_TARGET} PRIVATE "${arg_OUTPUT_FILE}")

  set_source_files_properties("${arg_INPUT_FILE}" TARGET_DIRECTORY ${arg_TARGET} PROPERTIES HEADER_FILE_ONLY ON)
  target_sources(${arg_TARGET} PRIVATE "${arg_INPUT_FILE}")
endfunction()
