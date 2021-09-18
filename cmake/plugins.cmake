function(add_plugin PLUGIN_NAME)
  set(options "")
  set(singleValues DIRECTORY)
  set(multiValues SOURCES DEFINITIONS LIBRARIES)
  cmake_parse_arguments(PLUGIN "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  if (NOT DEFINED PLUGIN_DIRECTORY)
    set(PLUGIN_DIRECTORY ${PLUGIN_NAME})
  endif()
  if(DEFINED PLUGIN_SOURCES)
    list(TRANSFORM PLUGIN_SOURCES PREPEND ${PLUGIN_DIRECTORY}/)
  endif()

  add_library(wxFormBuilder_${PLUGIN_NAME} MODULE)
  add_library(wxFormBuilder::${PLUGIN_NAME} ALIAS wxFormBuilder_${PLUGIN_NAME})
  set_target_properties(wxFormBuilder_${PLUGIN_NAME} PROPERTIES
      OUTPUT_NAME $<$<BOOL:${MSVC}>:lib>${PLUGIN_NAME}
  )

  target_sources(wxFormBuilder_${PLUGIN_NAME}
    PRIVATE
      ${PLUGIN_DIRECTORY}/${PLUGIN_NAME}.cpp
  )
  if(DEFINED PLUGIN_SOURCES)
    target_sources(wxFormBuilder_${PLUGIN_NAME}
      PRIVATE
        ${PLUGIN_SOURCES}
    )
  endif()
  if(DEFINED PLUGIN_DEFINITIONS)
    target_compile_definitions(wxFormBuilder_${PLUGIN_NAME}
      PRIVATE
        ${PLUGIN_DEFINITIONS}
    )
  endif()
  target_link_libraries(wxFormBuilder_${PLUGIN_NAME}
    PUBLIC
      wxFormBuilder::plugin-interface
  )
  if(DEFINED PLUGIN_LIBRARIES)
    target_link_libraries(wxFormBuilder_${PLUGIN_NAME}
      PRIVATE
        ${PLUGIN_LIBRARIES}
    )
  endif()

  if(APPLE)
    set_target_properties(wxFormBuilder_${PLUGIN_NAME} PROPERTIES
      INSTALL_RPATH "@loader_path/../Frameworks"
    )
  else()
    set_target_properties(wxFormBuilder_${PLUGIN_NAME} PROPERTIES
      INSTALL_RPATH $ORIGIN/..
    )
  endif()

  if(WIN32)
    # TODO: Using RUNTIME_DEPENDENCIES results in the dependencies getting installed in the locations
    #       defined in this command, this is however unwanted because they should get installed in
    #       the global location. Registering the dependencies in a set and installing them later with
    #       the standalone command has the desired effect, but why?
    install(TARGETS wxFormBuilder_${PLUGIN_NAME}
      RUNTIME_DEPENDENCY_SET wxFormBuilder_deps
      RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}/plugins/${PLUGIN_NAME}
      LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/plugins/${PLUGIN_NAME}
    )
  elseif(APPLE)
    install(TARGETS wxFormBuilder_${PLUGIN_NAME}
      RUNTIME
        DESTINATION wxFormBuilder.app/Contents/PlugIns
      LIBRARY
        DESTINATION wxFormBuilder.app/Contents/PlugIns
    )
  else()
    install(TARGETS wxFormBuilder_${PLUGIN_NAME}
      RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}/wxformbuilder
      LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/wxformbuilder
    )
  endif()
  # TODO: After changing the layout, maybe use an own CMakeLists for resources and even own targets for them
  install(DIRECTORY ../output/plugins/${PLUGIN_DIRECTORY}/icons
    DESTINATION ${CMAKE_INSTALL_DATADIR}/plugins/${PLUGIN_NAME}
  )
  install(DIRECTORY ../output/plugins/${PLUGIN_DIRECTORY}/xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/plugins/${PLUGIN_NAME}
  )
endfunction()
