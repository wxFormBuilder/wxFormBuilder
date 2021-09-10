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
      OUTPUT_NAME $<$<BOOL:${WIN32}>:lib>${PLUGIN_NAME}
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
    PRIVATE
      wxFormBuilder::plugin-interface
  )
  if(DEFINED PLUGIN_LIBRARIES)
    target_link_libraries(wxFormBuilder_${PLUGIN_NAME}
      PRIVATE
        ${PLUGIN_LIBRARIES}
    )
  endif()
  set_target_properties(wxFormBuilder_${PLUGIN_NAME} PROPERTIES
    INSTALL_RPATH $ORIGIN/..
  )

  install(TARGETS wxFormBuilder_${PLUGIN_NAME}
    RUNTIME
      DESTINATION ${CMAKE_INSTALL_BINDIR}/wxformbuilder
    LIBRARY
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/wxformbuilder
  )
  # TODO: After changing the layout, maybe use an own CMakeLists for resources and even own targets for them
  install(DIRECTORY ../output/plugins/${PLUGIN_DIRECTORY}/icons
    DESTINATION ${CMAKE_INSTALL_DATADIR}/wxformbuilder/plugins/${PLUGIN_NAME}
  )
  install(DIRECTORY ../output/plugins/${PLUGIN_DIRECTORY}/xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/wxformbuilder/plugins/${PLUGIN_NAME}
  )
endfunction()
