project(wxFormBuilder)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
set(CMAKE_CXX_STANDARD 11)

find_package(wxWidgets 3.0.0 COMPONENTS gl aui adv html core net base xml stc richtext propgrid ribbon xrc REQUIRED)
include(${wxWidgets_USE_FILE})

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations ${CMAKE_CXX_FLAGS}")
endif()
set(CMAKE_CXX_FLAGS_DEBUG "-D__WXFB_DEBUG__ ${CMAKE_CXX_FLAGS_DEBUG}")

if(WIN32)
    set(INSTALL_BIN_DIR ".")
    set(INSTALL_SHARE_DIR ".")
else()
    set(INSTALL_BIN_DIR "bin")
    set(INSTALL_SHARE_DIR "share/wxformbuilder")
endif()

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${INSTALL_BIN_DIR})
include(InstallRequiredSystemLibraries)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/${INSTALL_BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib)

include_directories(sdk/ticpp)
add_definitions(-DTIXML_USE_TICPP)
add_subdirectory(src)
add_subdirectory(sdk/ticpp)
add_subdirectory(sdk/plugin_interface)
add_subdirectory(plugins)

