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
- GIT_REVISION: Revision description
]]

set(GIT_REVISION "")

find_program(git_cmd NAMES git)
if (git_cmd)
  execute_process(
    COMMAND "${git_cmd}" describe --long --dirty
    WORKING_DIRECTORY "${gitDirectory}"
    RESULT_VARIABLE git_result
    OUTPUT_VARIABLE GIT_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  # A non-zero return code means error
  if(git_result)
    message(WARNING "Git could not determine a revision by describe, trying rev-parse")
    execute_process(
      COMMAND "${git_cmd}" rev-parse --short HEAD
      WORKING_DIRECTORY "${gitDirectory}"
      RESULT_VARIABLE git_result
      OUTPUT_VARIABLE GIT_REVISION
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(git_result)
      if(gitRequired)
        message(FATAL_ERROR "Git could not determine a revision by rev-parse")
      else()
        message(WARNING "Git could not determine a revision by rev-parse, using empty revision")
      endif()
    else()
      # TODO: A hack for getStrippedRevision(const char*) to produce a leading dash
      string(PREPEND GIT_REVISION "-")
    endif()
  endif()
else()
  if(gitRequired)
    message(FATAL_ERROR "Git not found")
  else()
    message(WARNING "Git not found, using empty revision")
  endif()
endif()

configure_file("${gitTemplate}" "${gitOutput}" @ONLY)
