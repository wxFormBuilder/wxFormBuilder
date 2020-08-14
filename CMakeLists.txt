cmake_minimum_required(VERSION 3.15)
project(wxFormBuilder LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
set(CMAKE_CXX_STANDARD 17)
set(DEFAULT_BUILD_TYPE "Release")

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE
      STRING "Choose the type of build." FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(UNIX OR MSYS)
    set(wxWidgets_FIND_STYLE "unix")
endif()

find_package(wxWidgets 3.0 COMPONENTS gl aui adv html core net base xml stc richtext propgrid ribbon xrc REQUIRED)
include("${wxWidgets_USE_FILE}")

set(CPACK_PACKAGE_VERSION_MAJOR 3)
set(CPACK_PACKAGE_VERSION_MINOR 9)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/COPYING)
set(CPACK_WIX_UPGRADE_GUID B5AAB525-34D5-4B12-B6F2-136C8ED890C9)

include(CPack)

set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-D__WXFB_DEBUG__ ${CMAKE_CXX_FLAGS_DEBUG}")

set(INSTALL_ROOT_DIR "cmake_install")

if(MSYS)
    set(INSTALL_BIN_DIR ".")
    set(INSTALL_SHARE_DIR ".")
    set(INSTALL_LIB_DIR ".")
elseif(UNIX)
    set(INSTALL_BIN_DIR "bin")
    set(INSTALL_SHARE_DIR "share/wxformbuilder")
    set(INSTALL_LIB_DIR "lib/wxformbuilder")
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/${INSTALL_ROOT_DIR})

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${INSTALL_BIN_DIR})
include(InstallRequiredSystemLibraries)

include_directories(subprojects/ticpp)
add_definitions(-DTIXML_USE_TICPP)
add_subdirectory(src)
add_subdirectory(subprojects/ticpp)
add_subdirectory(sdk/plugin_interface)
add_subdirectory(plugins)

