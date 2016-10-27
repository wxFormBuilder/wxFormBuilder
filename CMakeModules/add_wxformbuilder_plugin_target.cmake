macro(add_wxformbuilder_plugin_target plugin_name)
    set(argn_var "${ARGN}")
    set(plugin_files "")
    foreach(plugin_file IN LISTS argn_var)
        set(plugin_files ${plugin_files} ${plugin_file})
    endforeach()

    add_library(${plugin_name} SHARED ${plugin_files})
    target_link_libraries(${plugin_name} wxfb-plugin-interface)

    add_custom_command(TARGET ${plugin_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/icons 
        ${PROJECT_SOURCE_DIR}/output/share/wxformbuilder/plugins/${plugin_name}/icons
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/xml 
        ${PROJECT_SOURCE_DIR}/output/share/wxformbuilder/plugins/${plugin_name}/xml
    )

    install(TARGETS ${plugin_name} LIBRARY DESTINATION lib/wxformbuilder)
    install(DIRECTORY icons xml DESTINATION share/wxformbuilder/plugins/${plugin_name} OPTIONAL)
endmacro(add_wxformbuilder_plugin_target)
