function(add_plugin PLUGIN_NAME)
  set(options "")
  set(singleValues DIRECTORY)
  set(multiValues SOURCES DEFINITIONS LIBRARIES COMPONENTS ICONS)
  cmake_parse_arguments(PLUGIN "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  if(NOT DEFINED PLUGIN_DIRECTORY)
    set(PLUGIN_DIRECTORY ${PLUGIN_NAME})
  endif()
  if(DEFINED PLUGIN_SOURCES)
    list(TRANSFORM PLUGIN_SOURCES PREPEND ${PLUGIN_DIRECTORY}/)
  endif()
  if(NOT DEFINED PLUGIN_COMPONENTS)
    set(PLUGIN_COMPONENTS ${PLUGIN_NAME})
  endif()
  if(DEFINED PLUGIN_ICONS)
    # TODO: In the final sources layout these files should reside next to the sources
    list(TRANSFORM PLUGIN_ICONS PREPEND ../output/plugins/${PLUGIN_DIRECTORY}/icons/)
  endif()

  add_library(wxFormBuilder_${PLUGIN_NAME} MODULE)
  add_library(wxFormBuilder::${PLUGIN_NAME} ALIAS wxFormBuilder_${PLUGIN_NAME})
  set_target_properties(wxFormBuilder_${PLUGIN_NAME} PROPERTIES
      OUTPUT_NAME ${PLUGIN_NAME}
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

  set(generatorLanguages cpp python lua php)
  set(PLUGIN_COMPONENTS_DEFINITIONS "")
  set(PLUGIN_COMPONENTS_TEMPLATES "")
  foreach(component IN LISTS PLUGIN_COMPONENTS)
    # TODO: In the final sources layout these files should reside next to the sources  
    list(APPEND PLUGIN_COMPONENTS_DEFINITIONS ../output/plugins/${PLUGIN_DIRECTORY}/xml/${component}.xml)
    foreach(lang IN LISTS generatorLanguages)
      # TODO: In the final sources layout these files should reside next to the sources
      list(APPEND PLUGIN_COMPONENTS_TEMPLATES ../output/plugins/${PLUGIN_DIRECTORY}/xml/${component}.${lang}code)
    endforeach()
  endforeach()

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

  install(FILES ${PLUGIN_COMPONENTS_DEFINITIONS}
    DESTINATION ${CMAKE_INSTALL_DATADIR}/plugins/${PLUGIN_NAME}/xml
  )
  install(FILES ${PLUGIN_COMPONENTS_TEMPLATES}
    DESTINATION ${CMAKE_INSTALL_DATADIR}/plugins/${PLUGIN_NAME}/xml
  )
  if(DEFINED PLUGIN_ICONS)
    # TODO: This loses a possible directory structure of the input files
    install(FILES ${PLUGIN_ICONS}
      DESTINATION ${CMAKE_INSTALL_DATADIR}/plugins/${PLUGIN_NAME}/icons
    )
  endif()
endfunction()
