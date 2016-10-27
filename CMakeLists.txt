project(wxFormBuilder)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
set(CMAKE_CXX_STANDARD 11)

find_package(wxWidgets 3.0.0 COMPONENTS gl aui adv html core net base xml stc richtext propgrid ribbon xrc REQUIRED)
include(${wxWidgets_USE_FILE})

set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-D__WXFB_DEBUG__ ${CMAKE_CXX_FLAGS_DEBUG}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib)

add_subdirectory(src)
add_subdirectory(sdk/ticpp)
add_subdirectory(sdk/plugin_interface)
add_subdirectory(plugins)

