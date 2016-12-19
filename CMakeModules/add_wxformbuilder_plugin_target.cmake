macro(add_wxformbuilder_plugin_target plugin_name)
    set(argn_var "${ARGN}")
    set(plugin_files "")
    foreach(plugin_file IN LISTS argn_var)
        set(plugin_files ${plugin_files} ${plugin_file})
    endforeach()

    add_library(${plugin_name} SHARED ${plugin_files})
    target_link_libraries(${plugin_name} wxfb-plugin-interface)
    target_compile_definitions(${plugin_name} PRIVATE BUILD_DLL)

    if(APPLE)
        add_custom_command(TARGET ${plugin_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/icons 
            $<TARGET_FILE_DIR:wxFormBuilder>/../${INSTALL_SHARE_DIR}/plugins/${plugin_name}/icons
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/xml 
            $<TARGET_FILE_DIR:wxFormBuilder>/../${INSTALL_SHARE_DIR}/plugins/${plugin_name}/xml
        )
    elseif(WIN32)
        add_custom_command(TARGET ${plugin_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/icons 
            $<TARGET_PROPERTY:wxFormBuilder,BINARY_DIR>/${INSTALL_SHARE_DIR}/plugins/${plugin_name}/icons
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/xml 
            $<TARGET_PROPERTY:wxFormBuilder,BINARY_DIR>/${INSTALL_SHARE_DIR}/plugins/${plugin_name}/xml
        )
    else()
        add_custom_command(TARGET ${plugin_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/icons 
            $<TARGET_FILE_DIR:wxFormBuilder>/${INSTALL_SHARE_DIR}/plugins/${plugin_name}/icons
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/xml 
            $<TARGET_FILE_DIR:wxFormBuilder>/${INSTALL_SHARE_DIR}/plugins/${plugin_name}/xml
        )
    endif()

    if(MSVC)
        set_target_properties(${plugin_name} PROPERTIES
            OUTPUT_NAME lib${plugin_name}
            RUNTIME_OUTPUT_DIRECTORY $<TARGET_PROPERTY:wxFormBuilder,BINARY_DIR>/${INSTALL_SHARE_DIR}/plugins/${plugin_name}$<0:_>
            )
    elseif(APPLE)
        set_target_properties(${plugin_name} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:wxFormBuilder>/../PlugIns
            LIBRARY_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:wxFormBuilder>/../PlugIns
            )
    endif()

    if(NOT APPLE)
        if(WIN32)
            set(plugin_target_dir ${INSTALL_SHARE_DIR}/plugins/${plugin_name})
        else()
            set(plugin_target_dir lib/wxformbuilder)
        endif()
        install(TARGETS ${plugin_name} LIBRARY DESTINATION ${plugin_target_dir} RUNTIME DESTINATION ${plugin_target_dir})
        install(DIRECTORY icons xml DESTINATION ${INSTALL_SHARE_DIR}/plugins/${plugin_name} OPTIONAL)
    endif()
endmacro(add_wxformbuilder_plugin_target)
