cmake_minimum_required(VERSION 3.15)
project(wxFormBuilder
    VERSION 3.9.1
    LANGUAGES CXX C
    DESCRIPTION "An Open Source GUI Builder for wxWidgets"
    HOMEPAGE_URL "http://wxformbuilder.org"
)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/scripts/cmake)

option(wxFB_DISABLE_MEDIACTRL "Disable wxMediaCtrl / wxMedia library. [Default: OFF]" OFF)

# TODO: Custom wxWidgets build
#option(wxFB_DISABLE_SHARED    "Use static wxWidgets build instead of shared libraries. [Default: OFF]" OFF)
#set(wxFB_WX_ROOT "" CACHE STRING "Install destination for VST bundle [Default: Empty]")

include(wxFBConfig)

if(${wxWidgets_FOUND})
    include(wxFBLibs)
    include(wxFormBuilder)

    message(STATUS "
CMake Generator:             ${CMAKE_GENERATOR}

Project name:                ${PROJECT_NAME}
Project version:             ${PROJECT_VERSION}
Build type:                  ${CMAKE_BUILD_TYPE}
Build processor:             ${wxFB_SYSTEM_PROCESSOR}
Use clang libc++:            ${wxFB_USE_LIBCPP}
Install prefix:              ${CMAKE_INSTALL_PREFIX}
Output directory:            ${CMAKE_BINARY_DIR}
Disable wxMediaCtrl:         ${wxFB_DISABLE_MEDIACTRL}

wxWidgets version:           ${wxWidgets_VERSION_STRING}
wxWidgets static:            ${wxWidgets_DEFAULT_STATIC}
wxWidgets debug:             ${wxWidgets_DEFAULT_DEBUG}
wxWidgets unicode:           ${wxWidgets_DEFAULT_UNICODE}
wxWidgets CXX flags:         ${wxWidgets_CXX_FLAGS_LIST}

Compiler CXX debug flags:    ${CMAKE_CXX_FLAGS_DEBUG}
Compiler CXX release flags:  ${CMAKE_CXX_FLAGS_RELEASE}
Compiler CXX min size flags: ${CMAKE_CXX_FLAGS_MINSIZEREL}
")
endif()

include(Utils)
#print_all_variables()
