#include(add_wxformbuilder_plugin_target)
include_directories(${CMAKE_SOURCE_DIR}/sdk/plugin_interface)

function(install_plugin plugin_name)
    if(MSYS)
        install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${plugin_name}${CMAKE_SHARED_LIBRARY_SUFFIX}
            DESTINATION ${INSTALL_LIB_DIR}/plugins/${plugin_name}/
        )
    elseif(UNIX)
        install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${plugin_name}${CMAKE_SHARED_LIBRARY_SUFFIX}
            DESTINATION ${INSTALL_LIB_DIR}
        )
    endif()
endfunction()

add_subdirectory(additional)
add_subdirectory(common)
add_subdirectory(containers)
add_subdirectory(forms)
add_subdirectory(layout)
