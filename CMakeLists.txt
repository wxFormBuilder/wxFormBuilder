cmake_minimum_required(VERSION 3.27...3.31)

# Do not change the format of this line! It must stay as-is up to and including the keyword LANGUAGES.
# External tools parse the version number from this line.
project(wxFormBuilder VERSION 4.2.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(cmake/functions.cmake)

wxfb_set_default_build_type()
wxfb_set_default_compiler_options()
wxfb_set_language_standard(CXX_STANDARD 17)
if(PROJECT_IS_TOP_LEVEL)
  set_property(GLOBAL PROPERTY USE_FOLDERS YES)
endif()

option(WXFB_WXWIDGETS_ENABLE_30 "Allow using wxWidgets version 3.0.x. Not recommended, only for platforms without wxWidgets version 3.2.x." OFF)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/tinyxml2/CMakeLists.txt")
  set(wxfbTinyxml2ProviderDefault static)
else()
  set(wxfbTinyxml2ProviderDefault system)
endif()
set(wxfbTinyxml2ProviderValues static shared system)
set(WXFB_TINYXML2_PROVIDER "${wxfbTinyxml2ProviderDefault}" CACHE STRING "Provider of TinyXML-2: static or shared build the bundled submodule, system uses the installed library.")
set_property(CACHE WXFB_TINYXML2_PROVIDER PROPERTY STRINGS ${wxfbTinyxml2ProviderValues})
if(NOT WXFB_TINYXML2_PROVIDER IN_LIST wxfbTinyxml2ProviderValues)
  message(FATAL_ERROR "Invalid value '${WXFB_TINYXML2_PROVIDER}' for WXFB_TINYXML2_PROVIDER, must be one of: ${wxfbTinyxml2ProviderValues}")
endif()
unset(wxfbTinyxml2ProviderDefault)
unset(wxfbTinyxml2ProviderValues)
option(WXFB_BUILD_PLUGIN_HOST "Build as plugin host. Disables build steps that must be performed by the toplevel project." OFF)
if(PROJECT_IS_TOP_LEVEL OR WXFB_BUILD_PLUGIN_HOST)
  set(wxfbStageBuildDefault ON)
else()
  # TODO: Maybe check if the required variables are all unset and enable by default in that case
  set(wxfbStageBuildDefault OFF)
endif()
option(WXFB_STAGE_BUILD "Use stage directory to build directly executable variant that doesn't need to be installed." ${wxfbStageBuildDefault})
unset(wxfbStageBuildDefault)
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
  set(WXFB_INSTALL_LIBDIR wxFormBuilder.app/Contents/Frameworks CACHE INTERNAL "")
  set(WXFB_INSTALL_DATADIR wxFormBuilder.app/Contents/SharedSupport CACHE INTERNAL "")
else()
  set(WXFB_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR} CACHE INTERNAL "")
  set(WXFB_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR} CACHE INTERNAL "")
  set(WXFB_INSTALL_DATADIR ${CMAKE_INSTALL_DATADIR}/wxformbuilder CACHE INTERNAL "")
endif()
# Define internal stage directory only if enabled
if(WXFB_STAGE_BUILD)
  get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
  set(WXFB_STAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/stage$<$<BOOL:${isMultiConfig}>:/$<CONFIG>>" CACHE INTERNAL "")
  unset(isMultiConfig)
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
  set(wxWidgetsVersion 3.2)
else()
  set(wxWidgetsVersion 3.0)
endif()
# TODO: OPTIONAL_COMPONENTS seems to be broken currently (at least on Linux), so make media a required component
set(wxWidgetsComponentsRequired media xrc richtext propgrid ribbon stc html aui core net xml base)
if(WXFB_WXWIDGETS_ENABLE_30)
  list(PREPEND wxWidgetsComponentsRequired adv)
endif()
find_package(wxWidgets ${wxWidgetsVersion} CONFIG COMPONENTS ${wxWidgetsComponentsRequired})
if(NOT wxWidgets_FOUND)
  find_package(wxWidgets ${wxWidgetsVersion} MODULE REQUIRED ${wxWidgetsComponentsRequired})
endif()
wxfb_create_target_wxwidgets(COMPONENTS ${wxWidgetsComponentsRequired})
unset(wxWidgetsVersion)
unset(wxWidgetsComponentsRequired)
restore_variable(BUILD_SHARED_LIBS)
# TODO: Some Boost packages do not include the CMake config files so config mode with the headers component can't be used
find_package(Boost 1.71.0 REQUIRED)

# Libraries found by Find Modules are not necessarily shared imported targets, but provide their library directories,
# the runtime libraries are expected to be located there. Store them globally, so they are available when the
# installation is configured by a plugin host.
set_property(GLOBAL PROPERTY WXFB_RUNTIME_DIRECTORIES ${wxWidgets_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

include(cmake/macos-bundle.cmake)
include(cmake/macros.cmake)
include(cmake/revision.cmake)
include(cmake/version.cmake)

wxfb_configure_prebuild()

# Must be done before the subdirectories, so the tinyxml2::tinyxml2 target is visible to them
if(WXFB_TINYXML2_PROVIDER STREQUAL "system")
  find_package(tinyxml2 10.0 CONFIG REQUIRED)
else()
  if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/tinyxml2/CMakeLists.txt")
    message(FATAL_ERROR "The TinyXML-2 submodule is missing, run 'git submodule update --init' or set WXFB_TINYXML2_PROVIDER to 'system'.")
  endif()
  if(WXFB_TINYXML2_PROVIDER STREQUAL "shared")
    set(wxfbTinyxml2SharedLibs ON)
  else()
    set(wxfbTinyxml2SharedLibs OFF)
  endif()
  stash_variable(CMAKE_POLICY_DEFAULT_CMP0177 VALUE NEW)
  stash_variable(BUILD_SHARED_LIBS VALUE ${wxfbTinyxml2SharedLibs})
  add_subdirectory(third_party/tinyxml2 EXCLUDE_FROM_ALL)
  restore_variable(BUILD_SHARED_LIBS)
  restore_variable(CMAKE_POLICY_DEFAULT_CMP0177)
  unset(wxfbTinyxml2SharedLibs)
endif()

add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(sdk)
add_subdirectory(third_party/md5)
add_subdirectory(third_party/stack_trace)

if(NOT WXFB_BUILD_PLUGIN_HOST)
  wxfb_configure_postbuild()
endif()
