# TICPP
add_subdirectory(subprojects/ticpp)
add_library(wxfb::ticpp ALIAS ticpp)

# Plugin Interface
set(wxFB_PLUGIN_INTERFACE_SOURCE_FILES
    sdk/plugin_interface/component.h
    sdk/plugin_interface/fontcontainer.h
    sdk/plugin_interface/plugin.h
    sdk/plugin_interface/xrcconv.h
    sdk/plugin_interface/xrcconv.cpp
    sdk/plugin_interface/forms/wizard.h
    sdk/plugin_interface/forms/wizard.cpp
    sdk/plugin_interface/forms/wizard.fbp
)
add_library(plugin_interface STATIC ${wxFB_PLUGIN_INTERFACE_SOURCE_FILES})
add_library(wxfb::plugin_interface ALIAS plugin_interface)
if(MSVC)
    # Workaround to unwanted build-type directory added by MSVC
    set_target_properties(plugin_interface PROPERTIES
        SUFFIX ".lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<0:>"
    )
endif()
target_link_libraries(plugin_interface wxfb::ticpp ${wxWidgets_LIBRARIES})

# Plugins
set(wxFBPlugins additional common containers forms layout)
function(wxfb_add_plugins)
    foreach(_plugin IN LISTS wxFBPlugins)
        add_library(${_plugin} MODULE "plugins/${_plugin}/${_plugin}.cpp")
        add_library(wxfb::${_plugin} ALIAS ${_plugin})
        if(WIN32)
            set_target_properties(${_plugin} PROPERTIES
                PREFIX "lib"
                SUFFIX ".dll"
#               LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/${_plugin}/$<0:>"
                LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/output/plugins/${_plugin}/$<0:>"
            )
        else()
            set_target_properties(${_plugin} PROPERTIES
#               LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/wxformbuilder"
                LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/output/lib/wxformbuilder"
            )
        endif()
        target_compile_definitions(${_plugin} PRIVATE BUILD_DLL)
        target_include_directories(${_plugin} PRIVATE "sdk/plugin_interface")
        target_link_libraries(${_plugin}
            wxfb::ticpp
            wxfb::plugin_interface
            ${wxWidgets_LIBRARIES}
        )
    endforeach()
endfunction()
wxfb_add_plugins()
