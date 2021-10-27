cmake_minimum_required(VERSION 3.20)

#[[
Retrieve revision details of the working copy.

NOTE: Currently only git is supported

get_revision(<target>
             INPUT_FILE <input-file>
             OUTPUT_FILE <output-file>
             [WORKING_DIRECTORY <working-dir>]
             [REQUIRED])

This function must be called at most one time for <target>.

The revision details are queried during the build phase, an utility target <target>-revision is created.

In the template file <input-file> the placeholders get replaced with revision information and the result
is written to <output-file>, for supported placeholders see revision-git.cmake. If <input-file> is not absolute
it is interpreted relative to ${CMAKE_CURRENT_SOURCE_DIR}, if <output-file> is not absolute it is interpreted
relative to ${CMAKE_CURRENT_BINARY_DIR}.

The revision details get queried from <working-dir>, if not specified ${PROJECT_SOURCE_DIR} is used.

If REQUIRED is specified it is an error if no revision details can be retrieved, otherwise it is just a warning
and the corresponding placeholders evaluate to empty values.
]]
function(get_revision arg_TARGET)
  set(options REQUIRED)
  set(singleValues INPUT_FILE OUTPUT_FILE WORKING_DIRECTORY)
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
  if(NOT DEFINED arg_WORKING_DIRECTORY)
    set(arg_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
  else()
    cmake_path(ABSOLUTE_PATH arg_WORKING_DIRECTORY BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE)
  endif()

  add_custom_target(${arg_TARGET}-revision
    COMMAND "${CMAKE_COMMAND}"
      -D "gitRequired=${arg_REQUIRED}"
      -D "gitDirectory=${arg_WORKING_DIRECTORY}"
      -D "gitTemplate=${arg_INPUT_FILE}"
      -D "gitOutput=${arg_OUTPUT_FILE}"
      -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/revision-git.cmake"
    BYPRODUCTS "${arg_OUTPUT_FILE}"
    COMMENT "${arg_TARGET}: Getting revision details"
    VERBATIM
  )
endfunction()
