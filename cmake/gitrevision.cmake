cmake_minimum_required(VERSION 3.21)

#[[
Determine git revision and generate target file from template file if it did change.

This script is meant to be executed during the build phase by CMake.

Input parameters:
- gitRequired: If true, it is an error if the git revision can't be determined
- gitDirectory: Working directory of the git command
- gitTemplate: Path to the template file
- gitOutput: Path to the generated file

Template parameters:
- GIT_REVISION: Revision short hash
- GIT_DESCRIBE: Long describe output
- GIT_TAG_NAME: Name of closest tag
- GIT_TAG_DISTANCE: Distance in commits to closest tag
- GIT_DIRTY: Dirty flag of working copy
]]

set(GIT_REVISION "")
set(GIT_DESCRIBE "")
set(GIT_TAG_NAME "")
set(GIT_TAG_DISTANCE "0")
set(GIT_DIRTY "false")

if(gitRequired)
  set(git_message_level FATAL_ERROR)
else()
  set(git_message_level WARNING)
endif()

find_program(git_cmd NAMES git)
if (git_cmd)
  execute_process(
    COMMAND "${git_cmd}" rev-parse --short HEAD
    WORKING_DIRECTORY "${gitDirectory}"
    RESULT_VARIABLE git_result
    OUTPUT_VARIABLE GIT_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(NOT git_result)
    execute_process(
      COMMAND "${git_cmd}" describe --long --dirty
      WORKING_DIRECTORY "${gitDirectory}"
      RESULT_VARIABLE git_result
      OUTPUT_VARIABLE GIT_DESCRIBE
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT git_result)
      execute_process(
        COMMAND "${git_cmd}" describe --abbrev=0
        WORKING_DIRECTORY "${gitDirectory}"
        RESULT_VARIABLE git_result
        OUTPUT_VARIABLE GIT_TAG_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
      )
      execute_process(
        COMMAND "${git_cmd}" rev-list --count "${GIT_TAG_NAME}..HEAD"
        WORKING_DIRECTORY "${gitDirectory}"
        RESULT_VARIABLE git_result
        OUTPUT_VARIABLE GIT_TAG_DISTANCE
        OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    else()
      # Currently describe is optional
      message(WARNING "Git could not describe")
    endif()
    execute_process(
      COMMAND "${git_cmd}" status --porcelain --ignore-submodules=none -unormal
      WORKING_DIRECTORY "${gitDirectory}"
      RESULT_VARIABLE git_result
      OUTPUT_VARIABLE git_discard
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_VARIABLE git_discard
      ERROR_STRIP_TRAILING_WHITESPACE
    )
    if(NOT git_result AND NOT git_discard)
      set(GIT_DIRTY "false")
    else()
      set(GIT_DIRTY "true")
    endif()
  else()
    message(${git_message_level} "Git could not determine a revision")
  endif()
else()
  message(${git_message_level} "Git not found")
endif()

configure_file("${gitTemplate}" "${gitOutput}" @ONLY)
