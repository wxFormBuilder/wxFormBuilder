cmake_minimum_required(VERSION 3.21)

# Do not change the format of this line! It must stay as-is up to and including the keyword LANGUAGES.
# External tools parse the version number from this line.
project(wxFormBuilder VERSION 3.10.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(cmake/functions.cmake)

wxfb_set_default_build_type()
wxfb_set_default_compiler_options()
wxfb_set_language_standard(CXX_STANDARD 17)
if(PROJECT_IS_TOP_LEVEL)
  set_property(GLOBAL PROPERTY USE_FOLDERS YES)
endif()

option(WXFB_WXWIDGETS_ENABLE_30 "Allow using wxWidgets version 3.0.x. Not recommended, only for platforms without wxWidgets version 3.1.x." OFF)
option(WXFB_TICPP_SHARED "Use shared variant of TiCPP." OFF)
option(WXFB_TINYXML2_SHARED "Use shared variant of TinyXML-2." OFF)
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
  set(wxWidgetsVersion 3.1.2)
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
stash_variable(BUILD_SHARED_LIBS ${WXFB_TINYXML2_SHARED})
add_subdirectory(third_party/tinyxml2)
restore_variable(BUILD_SHARED_LIBS)

if(NOT WXFB_BUILD_PLUGIN_HOST)
  wxfb_configure_postbuild()
endif()
