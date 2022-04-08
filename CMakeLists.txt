cmake_minimum_required(VERSION 3.21)

# Do not change the format of this line! It must stay as-is up to and including the keyword LANGUAGES.
# External tools parse the version number from this line.
project(wxFormBuilder VERSION 3.10.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(PROJECT_IS_TOP_LEVEL OR WXFB_BUILD_PLUGIN_HOST)
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

if(PROJECT_IS_TOP_LEVEL)
  # TODO: This is not the ideal solution to apply these warning flags.
  #       Toolchain files are not really intended for this purpose.
  #       Presets would be a viable solution, but currently, at least on VSCode, this
  #       has the side effect that pretty much everything from generator to build type
  #       to binary directory needs to be specified. The usual control ability of especially
  #       the build type gets lost in preset mode.
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    string(APPEND CMAKE_CXX_FLAGS " /W4")
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # This assumes the default Clang frontend and not the MSVC compatible one
    string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra -Wpedantic")
  else()
    # This assumes GCC
    string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra -Wpedantic")
  endif()
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
elseif(CMAKE_CXX_STANDARD LESS 17 OR CMAKE_CXX_STANDARD GREATER_EQUAL 98)
  message(FATAL_ERROR "The CMAKE_CXX_STANDARD value needs to be at least 17, current value: ${CMAKE_CXX_STANDARD}")
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (NOT DEFINED CMAKE_CXX_EXTENSIONS)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if(MSVC)
  # Disable warnings about standard conformant code that is not conform to Microsoft standards
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS)
  # Without BOM the UTF-8 encoding doesn't get detected so enforce it
  add_compile_options(/source-charset:utf-8)
endif()

if(MSVC OR MINGW AND NOT MSYS)
  set(wxWidgetsMonoDefault ON)
else()
  set(wxWidgetsMonoDefault OFF)
endif()
option(WXFB_WXWIDGETS_MONO "Use monolithic variant of wxWidgets." ${wxWidgetsMonoDefault})
unset(wxWidgetsMonoDefault)
option(WXFB_WXWIDGETS_ENABLE_30 "Allow using wxWidgets version 3.0.x. Not recommended, only for platforms without wxWidgets version 3.1.x." OFF)
option(WXFB_TICPP_SHARED "Use shared variant of TiCPP." OFF)
option(WXFB_BUILD_PLUGIN_HOST "Build as plugin host. Disables build steps that must be performed by the toplevel project." OFF)
if(PROJECT_IS_TOP_LEVEL OR WXFB_BUILD_PLUGIN_HOST)
  set(stageBuildDefault ON)
else()
  # TODO: Maybe check if the required variables are all unset and enable by default in that case
  set(stageBuildDefault OFF)
endif()
option(WXFB_STAGE_BUILD "Use stage directory to build directly executable variant that doesn't need to be installed." ${stageBuildDefault})
unset(stageBuildDefault)
option(WXFB_ENABLE_EXTRA_CHECKS "Enable additional static code analysis." OFF)

# TODO: Currently generator languages cannot be configured
set(WXFB_GENERATOR_LANGUAGES cpp python lua php CACHE INTERNAL "")

# Adjust paths to match wxStandardPaths implementation, use own cache variables to share across CMake scopes
include(GNUInstallDirs)
if(WIN32)
  set(WXFB_INSTALL_BINDIR . CACHE INTERNAL "")
  set(WXFB_INSTALL_LIBDIR . CACHE INTERNAL "")
  set(WXFB_INSTALL_DATADIR . CACHE INTERNAL "")
elseif(APPLE)
  set(WXFB_INSTALL_BINDIR . CACHE INTERNAL "")
  set(WXFB_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR} CACHE INTERNAL "")
  set(WXFB_INSTALL_DATADIR wxFormBuilder.app/Contents/SharedSupport CACHE INTERNAL "")
else()
  set(WXFB_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR} CACHE INTERNAL "")
  set(WXFB_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR} CACHE INTERNAL "")
  set(WXFB_INSTALL_DATADIR ${CMAKE_INSTALL_DATADIR}/wxformbuilder CACHE INTERNAL "")
endif()
# Define internal stage directory only if enabled
if(WXFB_STAGE_BUILD)
  set(WXFB_STAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/stage$<$<BOOL:${isMultiConfig}>:/$<CONFIG>>" CACHE INTERNAL "")
else()
  unset(WXFB_STAGE_DIR CACHE)
endif()

if(WXFB_ENABLE_EXTRA_CHECKS)
  set(extraOptions "")
  if(WIN32 AND NOT MSYS)
    list(APPEND extraOptions PATH_SUFFIXES LLVM/bin)
  endif()
  find_program(clang-tidy_cmd NAMES clang-tidy ${extraOptions} REQUIRED)
  mark_as_advanced(clang-tidy_cmd)
  unset(extraOptions)
  set(CMAKE_CXX_CLANG_TIDY "${clang-tidy_cmd}")
endif()

include(cmake/variable.cmake)

# BUILD_SHARED_LIBS does only affect win32 search style, wxWidgets_USE_STATIC does only affect unix search style
stash_variable(BUILD_SHARED_LIBS VALUE ON)
set(wxWidgets_USE_STATIC OFF)
if(NOT WXFB_WXWIDGETS_ENABLE_30)
  set(wxWidgetsVersion 3.1.2)
else()
  set(wxWidgetsVersion 3.0)
endif()
set(wxWidgetsComponentsRequired "")
if(WXFB_WXWIDGETS_MONO)
  list(APPEND wxWidgetsComponentsRequired mono)
else()
  # TODO: OPTIONAL_COMPONENTS seems to be broken currently (at least on Linux), so make media a required component
  list(APPEND wxWidgetsComponentsRequired media xrc richtext propgrid ribbon stc html aui xml net core base)
  if(WXFB_WXWIDGETS_ENABLE_30)
    list(APPEND wxWidgetsComponentsRequired adv)
  endif()
endif()
find_package(wxWidgets ${wxWidgetsVersion} REQUIRED ${wxWidgetsComponentsRequired})
# TODO: To easily share the used wxWidgets library beyond CMake scopes, instead of using the variables created by the find module,
#       create a target. This has also the advantage that not all targets in the current scope get polluted with wxWidgets settings.
#       Use the same approach like the file ${wxWidgets_USE_FILE}. Because of the results returned by the find module,
#       an import library can not be created (easily?), create an interface library instead.
#       This has the drawback that this library is always in global scope, this might make it difficult to use wxWidgets
#       with different components elsewhere.
if(PROJECT_IS_TOP_LEVEL OR WXFB_BUILD_PLUGIN_HOST)
  message(DEBUG "Creating target wxWidgets::wxWidgets")
  add_library(wxWidgets_wxWidgets INTERFACE)
  add_library(wxWidgets::wxWidgets ALIAS wxWidgets_wxWidgets)

  message(DEBUG "wxWidgets_INCLUDE_DIRS: ${wxWidgets_INCLUDE_DIRS}")
  if(wxWidgets_INCLUDE_DIRS)
    if(wxWidgets_INCLUDE_DIRS_NO_SYSTEM)
      target_include_directories(wxWidgets_wxWidgets INTERFACE ${wxWidgets_INCLUDE_DIRS})
    else()
      target_include_directories(wxWidgets_wxWidgets SYSTEM INTERFACE ${wxWidgets_INCLUDE_DIRS})
    endif()
  endif()
  message(DEBUG "wxWidgets_LIBRARY_DIRS: ${wxWidgets_LIBRARY_DIRS}")
  if(wxWidgets_LIBRARY_DIRS)
    target_link_directories(wxWidgets_wxWidgets INTERFACE ${wxWidgets_LIBRARY_DIRS})
  endif()
  message(DEBUG "wxWidgets_DEFINITIONS: ${wxWidgets_DEFINITIONS}")
  if(wxWidgets_DEFINITIONS)
    target_compile_definitions(wxWidgets_wxWidgets INTERFACE ${wxWidgets_DEFINITIONS})
  endif()
  message(DEBUG "wxWidgets_DEFINITIONS_DEBUG: ${wxWidgets_DEFINITIONS_DEBUG}")
  if(wxWidgets_DEFINITIONS_DEBUG)
    list(TRANSFORM wxWidgets_DEFINITIONS_DEBUG REPLACE [[^(.+)$]] [[$<$<CONFIG:Debug>:\1>]] OUTPUT_VARIABLE wxWidgets_DEFINITIONS_DEBUG_GENEX)
    message(DEBUG "wxWidgets_DEFINITIONS_DEBUG_GENEX: ${wxWidgets_DEFINITIONS_DEBUG_GENEX}")
    target_compile_definitions(wxWidgets_wxWidgets INTERFACE ${wxWidgets_DEFINITIONS_DEBUG_GENEX})
    unset(wxWidgets_DEFINITIONS_DEBUG_GENEX)
  endif()
  message(DEBUG "wxWidgets_CXX_FLAGS: ${wxWidgets_CXX_FLAGS}")
  if(wxWidgets_CXX_FLAGS)
    target_compile_options(wxWidgets_wxWidgets INTERFACE ${wxWidgets_CXX_FLAGS})
  endif()
  message(DEBUG "wxWidgets_LIBRARIES: ${wxWidgets_LIBRARIES}")
  if(wxWidgets_LIBRARIES)
    target_link_libraries(wxWidgets_wxWidgets INTERFACE ${wxWidgets_LIBRARIES})
  endif()
else()
  include(${wxWidgets_USE_FILE})
endif()
unset(wxWidgetsComponentsRequired)
unset(wxWidgetsVersion)
restore_variable(BUILD_SHARED_LIBS)
# TODO: Some Boost packages do not include the CMake config files so config mode with the headers component can't be used
find_package(Boost 1.71.0 REQUIRED)

include(cmake/functions.cmake)
include(cmake/macros.cmake)
include(cmake/revision.cmake)
include(cmake/version.cmake)

wxfb_configure_prebuild()

add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(sdk/plugin_interface)
add_subdirectory(third_party/md5)
add_subdirectory(third_party/stack_trace)
stash_variable(BUILD_SHARED_LIBS VALUE ${WXFB_TICPP_SHARED})
add_subdirectory(third_party/ticpp)
restore_variable(BUILD_SHARED_LIBS)

if(NOT WXFB_BUILD_PLUGIN_HOST)
  wxfb_configure_postbuild()
endif()
