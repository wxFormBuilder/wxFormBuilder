cmake_minimum_required(VERSION 3.20)

#[[
Determine svn revision and generate target file from template file if it did change.

This script is meant to be executed during the build phase by CMake.

Input parameters:
- scmRequired: If true, it is an error if the svn revision can't be determined
- scmDirectory: Working directory of the git command
- scmTemplate: Path to the template file
- scmOutput: Path to the generated file

Template parameters:
- SCM_REVISION: Revision
- SCM_DESCRIBE: Revision with dirty marker
- SCM_TAG_NAME: Always empty
- SCM_TAG_DISTANCE: Always zero
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

find_program(scm_cmd NAMES svnversion)
if(scm_cmd)
  execute_process(
    COMMAND "${scm_cmd}" -c -n
    WORKING_DIRECTORY "${scmDirectory}"
    RESULT_VARIABLE scm_result
    OUTPUT_VARIABLE scm_output
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(NOT scm_result)
    if(scm_output MATCHES [[^([0-9]+:)?([0-9]+)([MSP]*)$]])
      set(SCM_REVISION "${CMAKE_MATCH_2}")
      set(SCM_DESCRIBE "${CMAKE_MATCH_2}${CMAKE_MATCH_3}")
      if("${CMAKE_MATCH_3}" STREQUAL "")
        set(SCM_DIRTY "false")
      else()
        set(SCM_DIRTY "true")
      endif()
    else()
      message(${scm_message_level} "Svn could not parse revision")
    endif()
  else()
    message(${scm_message_level} "Svn could not determine a revision")
  endif()
else()
  message(${scm_message_level} "Svn not found")
endif()

configure_file("${scmTemplate}" "${scmOutput}" @ONLY)
