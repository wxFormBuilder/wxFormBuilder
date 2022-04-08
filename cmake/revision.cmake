cmake_minimum_required(VERSION 3.20)

#[[
Retrieve revision details of the working copy.

get_revision(<target>
             INPUT_FILE <input-file>
             OUTPUT_FILE <output-file>
             [WORKING_DIRECTORY <working-dir>]
             [SCM_TYPE <scm-type>]
             [FOLDER <folder>]
             [REQUIRED])

This function must be called at most one time for <target>.

The revision details are queried during the build phase, an utility target <target>-revision is created.
If <folder> is specified, the utility target will be placed into that folder.

In the template file <input-file> the placeholders get replaced with revision information and the result
is written to <output-file>, for supported placeholders see revision-<scm-type>.cmake. If <input-file> is not absolute
it is interpreted relative to ${CMAKE_CURRENT_SOURCE_DIR}, if <output-file> is not absolute it is interpreted
relative to ${CMAKE_CURRENT_BINARY_DIR}.

The revision details get queried from <working-dir>, if not specified ${PROJECT_SOURCE_DIR} is used.
If <scm-type> is not specified, it is determined by examining <working-dir>.
Supported values for <scm-type>: git, svn, git-properties

If REQUIRED is specified it is an error if no revision details can be retrieved, otherwise it is just a warning
and the corresponding placeholders evaluate to default values (zero / empty / false).

The <input-file>, <output-file> get added as source to <target>.
]]
function(get_revision arg_TARGET)
  set(options REQUIRED)
  set(singleValues INPUT_FILE OUTPUT_FILE WORKING_DIRECTORY SCM_TYPE FOLDER)
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
  set(validScmTypes git svn git-properties)
  if(NOT DEFINED arg_SCM_TYPE)
    foreach(scm IN LISTS validScmTypes)
      if(EXISTS "${arg_WORKING_DIRECTORY}/.${scm}")
        set(arg_SCM_TYPE ${scm})
        break()
      endif()
    endforeach()
    if(NOT DEFINED arg_SCM_TYPE)
      if(arg_REQUIRED)
        message(FATAL_ERROR "Cannot determine scm-type")
      else()
        message(WARNING "No scm information present")
        set(arg_SCM_TYPE none)
      endif()
    endif()
  else()
    if(NOT arg_SCM_TYPE IN_LIST validScmTypes)
      message(FATAL_ERROR "Unsupported scm-type: ${arg_SCM_TYPE}")
    endif()
  endif()

  add_custom_target(${arg_TARGET}-revision
    COMMAND "${CMAKE_COMMAND}"
      -D "scmRequired=${arg_REQUIRED}"
      -D "scmDirectory=${arg_WORKING_DIRECTORY}"
      -D "scmTemplate=${arg_INPUT_FILE}"
      -D "scmOutput=${arg_OUTPUT_FILE}"
      -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/revision-${arg_SCM_TYPE}.cmake"
    BYPRODUCTS "${arg_OUTPUT_FILE}"
    COMMENT "${arg_TARGET}: Getting revision details"
    VERBATIM
  )
  if(DEFINED arg_FOLDER)
    set_target_properties(${arg_TARGET}-revision PROPERTIES FOLDER "${arg_FOLDER}")
  endif()
  target_sources(${arg_TARGET} PRIVATE "${arg_OUTPUT_FILE}")

  set_source_files_properties("${arg_INPUT_FILE}" TARGET_DIRECTORY ${arg_TARGET} PROPERTIES HEADER_FILE_ONLY ON)
  target_sources(${arg_TARGET} PRIVATE "${arg_INPUT_FILE}")
endfunction()
