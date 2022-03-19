cmake_minimum_required(VERSION 3.21)

# Do not change the format of this line! It must stay as-is up to and including the keyword LANGUAGES.
# External tools parse the version number from this line.
project(wxFormBuilder VERSION 3.10.1 LANGUAGES CXX)

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

  set_property(GLOBAL PROPERTY USE_FOLDERS YES)
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

if(MSVC)
  # Disable warnings about standard conformant code that is not conform to Microsoft standards
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS)
endif()

if(MSVC OR MINGW AND NOT MSYS)
  set(wxfbWxWidgetsMonoDefault ON)
else()
  set(wxfbWxWidgetsMonoDefault OFF)
endif()
option(WXFB_WXWIDGETS_MONO "Use monolithic variant of wxWidgets." ${wxfbWxWidgetsMonoDefault})
option(WXFB_WXWIDGETS_ENABLE_30 "Allow using wxWidgets version 3.0.x. Not recommended, only for platforms without wxWidgets version 3.1.x." OFF)
option(WXFB_TICPP_SHARED "Use shared variant of TiCPP." OFF)
if(PROJECT_IS_TOP_LEVEL)
  set(wxfbStageBuildDefault ON)
else()
    # TODO: Maybe check if the required variables are all unset and enable by default in that case
  set(wxfbStageBuildDefault OFF)
endif()
option(WXFB_STAGE_BUILD "Use stage directory to build directly executable variant that doesn't need to be installed." ${wxfbStageBuildDefault})

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
set(BUILD_SHARED_LIBS ON)
set(wxWidgets_USE_STATIC OFF)
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
if(wxWidgets_FOUND AND NOT TARGET wxWidgets::wxWidgets)
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
endif()
if(DEFINED backup_BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ${backup_BUILD_SHARED_LIBS})
  unset(backup_BUILD_SHARED_LIBS)
else()
  unset(BUILD_SHARED_LIBS)
endif()
# TODO: Some Boost packages do not include the CMake config files so config mode with the headers component can't be used
find_package(Boost 1.71.0 REQUIRED)

include(cmake/functions.cmake)
include(cmake/revision.cmake)

# Set own, adjusted, installation paths
set(CMAKE_INSTALL_BINDIR ${WXFB_INSTALL_BINDIR})
set(CMAKE_INSTALL_LIBDIR ${WXFB_INSTALL_LIBDIR})
set(CMAKE_INSTALL_DATADIR ${WXFB_INSTALL_DATADIR})
# Use installation layout for staged build
if(WXFB_STAGE_BUILD)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${WXFB_STAGE_DIR}/${CMAKE_INSTALL_BINDIR}")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${WXFB_STAGE_DIR}/${CMAKE_INSTALL_LIBDIR}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${WXFB_STAGE_DIR}/${CMAKE_INSTALL_LIBDIR}")
endif()

add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(sdk/plugin_interface)
add_subdirectory(third_party/md5)
add_subdirectory(third_party/stack_trace)
if(DEFINED BUILD_SHARED_LIBS)
  set(backup_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
endif()
set(BUILD_SHARED_LIBS ${WXFB_TICPP_SHARED})
add_subdirectory(third_party/ticpp)
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
