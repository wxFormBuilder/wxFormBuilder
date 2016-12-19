project(wxFormBuilder)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
set(CMAKE_CXX_STANDARD 11)

find_package(wxWidgets 3.0.0 COMPONENTS gl aui adv html core net base xml stc richtext propgrid ribbon xrc REQUIRED)
include(${wxWidgets_USE_FILE})

set(CPACK_PACKAGE_VERSION_MAJOR 3)
set(CPACK_PACKAGE_VERSION_MINOR 6)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/license.txt)
set(CPACK_WIX_UPGRADE_GUID B5AAB525-34D5-4B12-B6F2-136C8ED890C9)

if(APPLE)
    set(CPACK_SYSTEM_NAME macOS)
endif()

include(CPack)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations ${CMAKE_CXX_FLAGS}")
endif()
set(CMAKE_CXX_FLAGS_DEBUG "-D__WXFB_DEBUG__ ${CMAKE_CXX_FLAGS_DEBUG}")

if(WIN32)
    set(INSTALL_BIN_DIR ".")
    set(INSTALL_SHARE_DIR ".")
elseif(APPLE)
    set(INSTALL_BIN_DIR ".")
    set(INSTALL_SHARE_DIR "SharedSupport")
else()
    set(INSTALL_BIN_DIR "bin")
    set(INSTALL_SHARE_DIR "share/wxformbuilder")
endif()

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${INSTALL_BIN_DIR})
include(InstallRequiredSystemLibraries)

include_directories(sdk/ticpp)
add_definitions(-DTIXML_USE_TICPP)
add_subdirectory(src)
add_subdirectory(sdk/ticpp)
add_subdirectory(sdk/plugin_interface)
add_subdirectory(plugins)

