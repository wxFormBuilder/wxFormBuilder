cmake_minimum_required(VERSION 3.20)

#[[
Extract stored git information from file .git-properties and generate target file from template file if it did change.

This script is meant to be executed during the build phase by CMake.

Input parameters:
- scmRequired: If true, it is an error if the git revision can't be determined
- scmDirectory: Working directory that contains the file .git-properties
- scmTemplate: Path to the template file
- scmOutput: Path to the generated file

Template parameters:
- SCM_REVISION: Revision short hash
- SCM_DESCRIBE: Long describe output
- SCM_TAG_NAME: Name of closest tag
- SCM_TAG_DISTANCE: Distance in commits to closest tag
- SCM_DIRTY: Always false
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

cmake_path(SET scm_file NORMALIZE "${scmDirectory}/.git-properties")
if(EXISTS "${scm_file}")
  file(STRINGS "${scm_file}" scm_file_content ENCODING UTF-8)
  foreach(scm_line IN LISTS scm_file_content)
    if(scm_line MATCHES [[^git\.commit\.id\.abbrev=([0-9a-fA-F]+)$]])
      set(SCM_REVISION "${CMAKE_MATCH_1}")  
      continue()
    endif()
    if(scm_line MATCHES [[^git\.commit\.id\.describe=([^-]+)(-([0-9]+)-g([0-9a-fA-F]+))?$]])
      if(CMAKE_MATCH_2)
        set(SCM_DESCRIBE "${CMAKE_MATCH_1}${CMAKE_MATCH_2}")
      endif()
      set(SCM_TAG_NAME "${CMAKE_MATCH_1}")
      if(CMAKE_MATCH_3)
        set(SCM_TAG_DISTANCE "${CMAKE_MATCH_3}")
      else()
        set(SCM_TAG_DISTANCE "0")
      endif()
      continue()
    endif()
    # Don't fail on unknown contents
    message(WARNING "Git information file contents unknown: ${scm_line}")
  endforeach()
  if(NOT SCM_REVISION)
    message(${scm_message_level} "Git information file contains no revision")
  endif()
  # Construct a missing long description at end of loop to be independent of the order of file entries
  if(SCM_TAG_NAME AND NOT SCM_DESCRIBE)
    set(SCM_DESCRIBE "${SCM_TAG_NAME}-${SCM_TAG_DISTANCE}-g${SCM_REVISION}")
  endif()
else()
  message(${scm_message_level} "Git information file not found: ${scm_file}")
endif()

configure_file("${scmTemplate}" "${scmOutput}" @ONLY)
