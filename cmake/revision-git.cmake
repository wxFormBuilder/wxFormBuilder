cmake_minimum_required(VERSION 3.20)

#[[
Determine git revision and generate target file from template file if it did change.

This script is meant to be executed during the build phase by CMake.

Input parameters:
- scmRequired: If true, it is an error if the git revision can't be determined
- scmDirectory: Working directory of the git command
- scmTemplate: Path to the template file
- scmOutput: Path to the generated file

Template parameters:
- SCM_REVISION: Revision short hash
- SCM_DESCRIBE: Long describe output
- SCM_TAG_NAME: Name of closest tag
- SCM_TAG_DISTANCE: Distance in commits to closest tag
- SCM_DIRTY: Dirty flag of working copy
]]

set(SCM_REVISION "0")
set(SCM_DESCRIBE "")
set(SCM_TAG_NAME "")
set(SCM_TAG_DISTANCE "0")
set(SCM_DIRTY "false")

if(scmRequired)
  set(scm_message_level FATAL_ERROR)
else()
  set(scm_message_level WARNING)
endif()

find_program(scm_cmd NAMES git)
if (scm_cmd)
  execute_process(
    COMMAND "${scm_cmd}" rev-parse --short HEAD
    WORKING_DIRECTORY "${scmDirectory}"
    RESULT_VARIABLE scm_result
    OUTPUT_VARIABLE SCM_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(NOT scm_result)
    execute_process(
      COMMAND "${scm_cmd}" describe --long --dirty
      WORKING_DIRECTORY "${scmDirectory}"
      RESULT_VARIABLE scm_result
      OUTPUT_VARIABLE SCM_DESCRIBE
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT scm_result)
      execute_process(
        COMMAND "${scm_cmd}" describe --abbrev=0
        WORKING_DIRECTORY "${scmDirectory}"
        RESULT_VARIABLE scm_result
        OUTPUT_VARIABLE SCM_TAG_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
      )
      execute_process(
        COMMAND "${scm_cmd}" rev-list --count "${SCM_TAG_NAME}..HEAD"
        WORKING_DIRECTORY "${scmDirectory}"
        RESULT_VARIABLE scm_result
        OUTPUT_VARIABLE SCM_TAG_DISTANCE
        OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    else()
      # Currently describe is optional
      message(WARNING "Git could not describe")
    endif()
    execute_process(
      COMMAND "${scm_cmd}" status --porcelain --ignore-submodules=none -unormal
      WORKING_DIRECTORY "${scmDirectory}"
      RESULT_VARIABLE scm_result
      OUTPUT_VARIABLE scm_discard
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_VARIABLE scm_discard
      ERROR_STRIP_TRAILING_WHITESPACE
    )
    if(NOT scm_result AND NOT scm_discard)
      set(SCM_DIRTY "false")
    else()
      set(SCM_DIRTY "true")
    endif()
  else()
    message(${scm_message_level} "Git could not determine a revision")
  endif()
else()
  message(${scm_message_level} "Git not found")
endif()

configure_file("${scmTemplate}" "${scmOutput}" @ONLY)
