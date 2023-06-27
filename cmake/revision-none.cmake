cmake_minimum_required(VERSION 3.20)

#[[
Dummy scm script to always return zero values.

This script is meant to be executed during the build phase by CMake.

Input parameters:
- scmTemplate: Path to the template file
- scmOutput: Path to the generated file

Template parameters:
- SCM_REVISION: Always zero
- SCM_BRANCH: Always empty
- SCM_DESCRIBE: Always empty
- SCM_TAG_NAME: Always empty
- SCM_TAG_DISTANCE: Always zero
- SCM_DIRTY: Always false
]]

set(SCM_REVISION "0")
set(SCM_BRANCH "")
set(SCM_DESCRIBE "")
set(SCM_TAG_NAME "")
set(SCM_TAG_DISTANCE "0")
set(SCM_DIRTY "false")

configure_file("${scmTemplate}" "${scmOutput}" @ONLY)
