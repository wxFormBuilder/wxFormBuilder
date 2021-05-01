# wxFB_DISABLE_MEDIACTRL
if(UNIX AND NOT wxFB_DISABLE_MEDIACTRL)
    execute_process(
        COMMAND wx-config --libs all
        OUTPUT_VARIABLE _wxLibs
    )
    if(_wxLibs MATCHES "media")
        add_compile_definitions(USE_MEDIACTRL)
    else()
        set(wxFB_DISABLE_MEDIACTRL ON)
    endif()
endif()

if(WIN32 AND NOT MSYS)
    list(APPEND wxLibsList gl core base net xml xrc html adv stc richtext propgrid ribbon aui)
    if(NOT wxFB_DISABLE_MEDIACTRL)
        list(APPEND wxLibsList media)
    endif()
else()
    list(APPEND wxLibsList all)
endif()

find_package(wxWidgets 3.0.3 REQUIRED ${wxLibsList})
if(${wxWidgets_FOUND})
    include(${wxWidgets_USE_FILE})
    include(CMakeDependentOption)

    set(CMAKE_CXX_FLAGS_DEBUG "-D__WXFB_DEBUG__ ${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    if(MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    endif()

    # The variable CMAKE_SYSTEM_PROCESSOR is incorrect on Visual studio, see
    # https://gitlab.kitware.com/cmake/cmake/issues/15170
    if(NOT wxFB_SYSTEM_PROCESSOR)
        if(MSVC)
            set(wxFB_SYSTEM_PROCESSOR "${MSVC_CXX_ARCHITECTURE_ID}")
        else()
            set(wxFB_SYSTEM_PROCESSOR "${CMAKE_SYSTEM_PROCESSOR}")
        endif()
    endif()

    # wxFB_USE_LIBCPP: libc++ is enabled by default on macOS.
    cmake_dependent_option(wxFB_USE_LIBCPP "Use libc++ with clang" "${APPLE}"
        "CMAKE_CXX_COMPILER_ID MATCHES Clang" OFF)
    if(wxFB_USE_LIBCPP)
        add_compile_options(-stdlib=libc++)
        if(CMAKE_VERSION VERSION_LESS 3.13)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
        else()
            add_link_options(-stdlib=libc++ -lc++abi)
        endif()
    endif()
endif()
