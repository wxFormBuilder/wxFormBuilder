#[[
This file can be used as top level CMakeLists.txt file to build a wxFormBuilder plugin.

This file should be added to a new repository, the wxFormBuilder project is required
for building and should be added as a git submodule.
]]
cmake_minimum_required(VERSION 3.21)

project(wxFormBuilderSDK VERSION 1.0.0 LANGUAGES CXX)

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(PROJECT_IS_TOP_LEVEL)
  set(buildTypes "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
  set(defaultBuildType "Debug")
  if(NOT isMultiConfig)
    if(NOT CMAKE_BUILD_TYPE)
      message(STATUS "Setting build type to '${defaultBuildType}' as none was specified.")
      set(CMAKE_BUILD_TYPE "${defaultBuildType}" CACHE STRING "Choose the type of build." FORCE)
      set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${buildTypes}")
    elseif(NOT CMAKE_BUILD_TYPE IN_LIST buildTypes)
      message(FATAL_ERROR "Unknown build type: '${CMAKE_BUILD_TYPE}'")
    endif()
  endif()
  unset(defaultBuildType)
  unset(buildTypes)

  set_property(GLOBAL PROPERTY USE_FOLDERS YES)
endif()

# The wxFormBuilder project must be processed first.
# This loads necessary CMake scripts and initializes internal variables.
# Adjust the path to the actual location of the wxFormBuilder submodule.
set(WXFB_BUILD_PLUGIN_HOST ON)
add_subdirectory(wxFormBuilder)

# Initialize the environment for building, this must be executed as first step.
wxfb_configure_prebuild()

# If you need to build external code or find libraries for your plugin, add such code here.
# If you need to link against the wxWidgets library, use the target wxWidgets::wxWidgets
# to use the same version used by wxFormBuilder.

# Each plugin gets fully defined by wxfb_add_plugin(), for a reference of available parameters
# refer to its documentation.
wxfb_add_plugin(example)

# Perform postbuild configuration steps, this must be executed as last step.
wxfb_configure_postbuild()
