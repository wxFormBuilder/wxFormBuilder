cmake_minimum_required(VERSION 3.21)

# Do not change the format of this line! It must stay as-is up to and including the keyword LANGUAGES.
# External tools parse the version number from this line.
project(wxFormBuilder VERSION 3.10.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

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
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
elseif(CMAKE_CXX_STANDARD LESS 17)
  message(FATAL_ERROR "The CMAKE_CXX_STANDARD value needs to be at least 17, current value: ${CMAKE_CXX_STANDARD}")
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (NOT DEFINED CMAKE_CXX_EXTENSIONS)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
if(APPLE)
  # TODO: The current plugin loader code requires this extension
  set(CMAKE_SHARED_MODULE_SUFFIX ".dylib")
endif()

if(MSVC OR MINGW AND NOT MSYS)
  set(wxWidgetsMonoDefault ON)
else()
  set(wxWidgetsMonoDefault OFF)
endif()
option(WXFB_WXWIDGETS_MONO "Use monolithic variant of wxWidgets." ${wxWidgetsMonoDefault})
option(WXFB_WXWIDGETS_SHARED "Use shared variant of wxWidgets." ON)
option(WXFB_WXWIDGETS_ENABLE_30 "Allow using wxWidgets version 3.0.x. Not recommended, only for platforms without wxWidgets version 3.1.x." OFF)
option(WXFB_TICPP_SHARED "Use shared variant of TiCPP." OFF)
if(PROJECT_IS_TOP_LEVEL)
  set(wxfbStagedBuildDefault ON)
else()
  set(wxfbStagedBuildDefault OFF)
endif()
option(WXFB_STAGE_BUILD "Use stage directory to build directly executable variant that doesn't need to be installed." ${wxfbStagedBuildDefault})

if(WXFB_STAGE_BUILD)
  set(WXFB_STAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/stage$<$<BOOL:${isMultiConfig}>:/$<CONFIG>>" CACHE INTERNAL "")
else()
  unset(WXFB_STAGE_DIR CACHE)
endif()
# TODO: Currently generator languages cannot be configured
set(WXFB_GENERATOR_LANGUAGES cpp python lua php CACHE INTERNAL "")

if(NOT WXFB_WXWIDGETS_ENABLE_30)
  set(wxWidgetsVersion 3.1.2)
else()
  set(wxWidgetsVersion 3.0)
endif()
# BUILD_SHARED_LIBS does only affect win32 search style, wxWidgets_USE_STATIC does only affect unix search style
if(DEFINED BUILD_SHARED_LIBS)
  set(backup_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
endif()
set(BUILD_SHARED_LIBS ${WXFB_WXWIDGETS_SHARED})
if(WXFB_WXWIDGETS_SHARED)
  set(wxWidgets_USE_STATIC OFF)
else()
  set(wxWidgets_USE_STATIC ON)
endif()
set(wxWidgetsComponentsRequired "")
if(WXFB_WXWIDGETS_MONO)
  list(APPEND wxWidgetsComponentsRequired mono scintilla)
else()
  # TODO: OPTIONAL_COMPONENTS seems to be broken currently (at least on Linux), so make media a required component
  list(APPEND wxWidgetsComponentsRequired media xrc richtext propgrid ribbon stc html aui xml net core base)
  if(WXFB_WXWIDGETS_ENABLE_30)
    list(APPEND wxWidgetsComponentsRequired adv)
  endif()
endif()
find_package(wxWidgets ${wxWidgetsVersion} REQUIRED ${wxWidgetsComponentsRequired})
include(${wxWidgets_USE_FILE})
if(DEFINED backup_BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ${backup_BUILD_SHARED_LIBS})
  unset(backup_BUILD_SHARED_LIBS)
else()
  unset(BUILD_SHARED_LIBS)
endif()
# TODO: Some Boost packages do not include the CMake config files so config mode with the headers component can't be used
find_package(Boost 1.71.0 REQUIRED)

# Adjust paths to match wxStandardPaths implementation
include(GNUInstallDirs)
if(WIN32)
  set(CMAKE_INSTALL_BINDIR .)
  set(CMAKE_INSTALL_LIBDIR .)
  set(CMAKE_INSTALL_DATADIR .)
elseif(APPLE)
  set(CMAKE_INSTALL_BINDIR .)
  set(CMAKE_INSTALL_DATADIR wxFormBuilder.app/Contents/SharedSupport)
else()
  set(CMAKE_INSTALL_DATADIR ${CMAKE_INSTALL_DATADIR}/wxformbuilder)
endif()

# Use installation layout for staged build
if(WXFB_STAGE_BUILD)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${WXFB_STAGE_DIR}/${CMAKE_INSTALL_BINDIR}")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${WXFB_STAGE_DIR}/${CMAKE_INSTALL_LIBDIR}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${WXFB_STAGE_DIR}/${CMAKE_INSTALL_LIBDIR}")
endif()

include(cmake/functions.cmake)
include(cmake/revision.cmake)

add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(sdk/plugin_interface)
if(DEFINED BUILD_SHARED_LIBS)
  set(backup_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
endif()
set(BUILD_SHARED_LIBS ${WXFB_TICPP_SHARED})
add_subdirectory(subprojects/ticpp)
if(DEFINED backup_BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ${backup_BUILD_SHARED_LIBS})
  unset(backup_BUILD_SHARED_LIBS)
else()
  unset(BUILD_SHARED_LIBS)
endif()

if(APPLE)
  # TODO: Is there any possibility to determine the output filenames of the plugins to create a list that can be inserted literal?
  # TODO: The fixup_bundle command erases all present RPATH information. Since it fixes the loader paths of all libraries and none
  #       of them loads libraries dynamically, it is sufficient to add an RPATH only to the PlugIns directory which contains the
  #       libraries that get loaded dynamically.
  install(
    CODE
    "
      include(BundleUtilities)
      file(GLOB moduleDependencies LIST_DIRECTORIES false \"\${CMAKE_INSTALL_PREFIX}/wxFormBuilder.app/Contents/PlugIns/*${CMAKE_SHARED_MODULE_SUFFIX}\")
      fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/wxFormBuilder.app\" \"\${moduleDependencies}\" \"\")

      find_program(install_name_tool_cmd NAMES install_name_tool REQUIRED)
      mark_as_advanced(install_name_tool_cmd)
      execute_process(COMMAND \"\${install_name_tool_cmd}\" -add_rpath @executable_path/../PlugIns \"\${CMAKE_INSTALL_PREFIX}/wxFormBuilder.app/Contents/MacOS/wxFormBuilder\")

      find_program(codesign_cmd NAMES codesign REQUIRED)
      mark_as_advanced(codesign_cmd)
      execute_process(COMMAND \"\${codesign_cmd}\" -s - -f --deep --verbose \"\${CMAKE_INSTALL_PREFIX}/wxFormBuilder.app\")
    "
  )
endif()
