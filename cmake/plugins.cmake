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

  if(WIN32)
    set(runtimeDestination ${CMAKE_INSTALL_BINDIR}/plugins/${PLUGIN_NAME})
    set(libraryDestination ${CMAKE_INSTALL_LIBDIR}/plugins/${PLUGIN_NAME})
    set(archiveDestination ${CMAKE_INSTALL_LIBDIR}/plugins/${PLUGIN_NAME})
  elseif(APPLE)
    set(runtimeDestination wxFormBuilder.app/Contents/PlugIns)
    set(libraryDestination wxFormBuilder.app/Contents/PlugIns)
    set(archiveDestination wxFormBuilder.app/Contents/PlugIns)
  else()
    set(runtimeDestination ${CMAKE_INSTALL_BINDIR}/wxformbuilder)
    set(libraryDestination ${CMAKE_INSTALL_LIBDIR}/wxformbuilder)
    set(archiveDestination ${CMAKE_INSTALL_LIBDIR}/wxformbuilder)
  endif()
  set(resourceDestination ${CMAKE_INSTALL_DATADIR}/plugins/${PLUGIN_NAME})

  if(WXFB_STAGE_BUILD)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${WXFB_STAGE_DIR}/${runtimeDestination}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${WXFB_STAGE_DIR}/${libraryDestination}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${WXFB_STAGE_DIR}/${archiveDestination}")
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

  # TODO: In the final sources layout these files should reside next to the sources
  set(sourceDir "${CMAKE_CURRENT_SOURCE_DIR}/../output/plugins/${PLUGIN_DIRECTORY}/xml")
  set(destDir "${WXFB_STAGE_DIR}/${resourceDestination}/xml")
  set(definitionsSource "")
  set(definitionsDest "")
  set(templatesSource "")
  set(templatesDest "")
  foreach(component IN LISTS PLUGIN_COMPONENTS)
    list(APPEND definitionsSource "${sourceDir}/${component}.xml")
    list(APPEND definitionsDest "${destDir}/${component}.xml")
    foreach(lang IN LISTS WXFB_GENERATOR_LANGUAGES)
      list(APPEND templatesSource "${sourceDir}/${component}.${lang}code")
      list(APPEND templatesDest "${destDir}/${component}.${lang}code")
    endforeach()
  endforeach()

  if(WXFB_STAGE_BUILD)
    add_custom_command(OUTPUT ${definitionsDest} COMMENT "wxFormBuilder_${PLUGIN_NAME}: Copying component definitions")
    foreach(source dest IN ZIP_LISTS definitionsSource definitionsDest)
      add_custom_command(
        OUTPUT ${definitionsDest}
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${source}" "${dest}"
        DEPENDS "${source}"
        VERBATIM
        APPEND
      )
    endforeach()
    add_custom_target(wxFormBuilder_${PLUGIN_NAME}-definitions
      DEPENDS ${definitionsDest}
    )

    add_custom_command(OUTPUT ${templatesDest} COMMENT "wxFormBuilder_${PLUGIN_NAME}: Copying code templates")
    foreach(source dest IN ZIP_LISTS templatesSource templatesDest)
      add_custom_command(
        OUTPUT ${templatesDest}
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${source}" "${dest}"
        DEPENDS "${source}"
        VERBATIM
        APPEND
      )
    endforeach()
    add_custom_target(wxFormBuilder_${PLUGIN_NAME}-templates
      DEPENDS ${templatesDest}
    )

    add_dependencies(wxFormBuilder_${PLUGIN_NAME}
      wxFormBuilder_${PLUGIN_NAME}-definitions
      wxFormBuilder_${PLUGIN_NAME}-templates
    )
  endif()

  if(DEFINED PLUGIN_ICONS)
    # TODO: In the final sources layout these files should reside next to the sources  
    set(sourceDir "${CMAKE_CURRENT_SOURCE_DIR}/../output/plugins/${PLUGIN_DIRECTORY}/icons")
    set(destDir "${WXFB_STAGE_DIR}/${resourceDestination}/icons")
    set(iconsSource "")
    set(iconsDest "")
    foreach(icon IN LISTS PLUGIN_ICONS)
      list(APPEND iconsSource "${sourceDir}/${icon}")
      list(APPEND iconsDest "${destDir}/${icon}")
    endforeach()

    if(WXFB_STAGE_BUILD)
      add_custom_command(OUTPUT ${iconsDest} COMMENT "wxFormBuilder_${PLUGIN_NAME}: Copying icons")
      foreach(source dest IN ZIP_LISTS iconsSource iconsDest)
        add_custom_command(
          OUTPUT ${iconsDest}
          COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${source}" "${dest}"
          DEPENDS "${source}"
          VERBATIM
          APPEND
        )
      endforeach()
      add_custom_target(wxFormBuilder_${PLUGIN_NAME}-icons
        DEPENDS ${iconsDest}
      )

      add_dependencies(wxFormBuilder_${PLUGIN_NAME}
        wxFormBuilder_${PLUGIN_NAME}-icons
      )
    endif()
  endif()

  # TODO: Using RUNTIME_DEPENDENCIES results in the dependencies getting installed in the locations
  #       defined in this command, this is however unwanted because they should get installed in
  #       the global location. Registering the dependencies in a set and installing them later with
  #       the standalone command has the desired effect, but why?
  install(TARGETS wxFormBuilder_${PLUGIN_NAME}
    RUNTIME_DEPENDENCY_SET wxFormBuilder_deps
    RUNTIME
      DESTINATION ${runtimeDestination}
    LIBRARY
      DESTINATION ${libraryDestination}
  )
  # TODO: In the final sources layout these files should reside next to the sources
  install(FILES ${definitionsSource}
    DESTINATION ${resourceDestination}/xml
  )
  install(FILES ${templatesSource}
    DESTINATION ${resourceDestination}/xml
  )
  if(DEFINED PLUGIN_ICONS)
    install(DIRECTORY ../output/plugins/${PLUGIN_DIRECTORY}/icons
      DESTINATION ${resourceDestination}
    )
  endif()
endfunction()


function(wxfb_target_definitions arg_TARGET)
  set(options "")
  set(singleValues INPUT_DIRECTORY OUTPUT_DIRECTORY INSTALL_DIRECTORY)
  set(multiValues COMMON TEMPLATES)
  cmake_parse_arguments(arg "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  if(NOT DEFINED arg_INPUT_DIRECTORY)
    message(FATAL_ERROR "No input directory specified")
  endif()

  cmake_path(ABSOLUTE_PATH arg_INPUT_DIRECTORY BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE OUTPUT_VARIABLE srcDir)
  cmake_path(APPEND srcDir "xml")
  if(DEFINED arg_OUTPUT_DIRECTORY)
    cmake_path(ABSOLUTE_PATH arg_OUTPUT_DIRECTORY BASE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" NORMALIZE OUTPUT_VARIABLE dstDir)
    cmake_path(APPEND dstDir "xml")
  endif()

  if(DEFINED arg_COMMON)
    set(itemSrc "")
    foreach(item IN LISTS arg_COMMON)
      list(APPEND itemSrc "${srcDir}/${item}.xml")
    endforeach()

    if(DEFINED arg_OUTPUT_DIRECTORY)
      set(itemDst "")
      foreach(item IN LISTS arg_COMMON)
        list(APPEND itemDst "${dstDir}/${item}.xml")
      endforeach()
      wxfb_copy_target_resources("${arg_TARGET}" "definitions" "${itemSrc}" "${itemDst}")
    endif()
    if(DEFINED arg_INSTALL_DIRECTORY)
      wxfb_install_files("${srcDir}" "${itemSrc}" "${arg_INSTALL_DIRECTORY}/xml")
    endif()
  endif()

  if(DEFINED arg_TEMPLATES)
    set(itemSrc "")
    foreach(item IN LISTS arg_TEMPLATES)
      foreach(lang IN LISTS WXFB_GENERATOR_LANGUAGES)
        list(APPEND itemSrc "${srcDir}/${item}.${lang}code")
      endforeach()
    endforeach()

    if(DEFINED arg_OUTPUT_DIRECTORY)
      set(itemDst "")
      foreach(item IN LISTS arg_TEMPLATES)
        foreach(lang IN LISTS WXFB_GENERATOR_LANGUAGES)
          list(APPEND itemDst "${dstDir}/${item}.${lang}code")
        endforeach()
      endforeach()
      wxfb_copy_target_resources("${arg_TARGET}" "templates" "${itemSrc}" "${itemDst}")
    endif()
    if(DEFINED arg_INSTALL_DIRECTORY)
      wxfb_install_files("${srcDir}" "${itemSrc}" "${arg_INSTALL_DIRECTORY}/xml")
    endif()
  endif()
endfunction()


function(wxfb_target_resources arg_TARGET)
  set(options "")
  set(singleValues INPUT_DIRECTORY OUTPUT_DIRECTORY INSTALL_DIRECTORY)
  set(multiValues RESOURCES ICONS)
  cmake_parse_arguments(arg "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  if(NOT DEFINED arg_INPUT_DIRECTORY)
    message(FATAL_ERROR "No input directory specified")
  endif()

  if(DEFINED arg_RESOURCES)
    cmake_path(ABSOLUTE_PATH arg_INPUT_DIRECTORY BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE OUTPUT_VARIABLE srcDir)
    set(itemSrc "")
    foreach(item IN LISTS arg_RESOURCES)
      list(APPEND itemSrc "${srcDir}/${item}")
    endforeach()

    if(DEFINED arg_OUTPUT_DIRECTORY)
      cmake_path(ABSOLUTE_PATH arg_OUTPUT_DIRECTORY BASE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" NORMALIZE OUTPUT_VARIABLE dstDir)
      set(itemDst "")
      foreach(item IN LISTS arg_RESOURCES)
        list(APPEND itemDst "${dstDir}/${item}")
      endforeach()
      wxfb_copy_target_resources("${arg_TARGET}" "resources" "${itemSrc}" "${itemDst}")
    endif()
    if(DEFINED arg_INSTALL_DIRECTORY)
      wxfb_install_files("${srcDir}" "${itemSrc}" "${arg_INSTALL_DIRECTORY}")
    endif()
  endif()

  if(DEFINED arg_ICONS)
    cmake_path(ABSOLUTE_PATH arg_INPUT_DIRECTORY BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE OUTPUT_VARIABLE srcDir)
    cmake_path(APPEND srcDir "icons")
    set(itemSrc "")
    foreach(item IN LISTS arg_ICONS)
      list(APPEND itemSrc "${srcDir}/${item}")
    endforeach()

    if(DEFINED arg_OUTPUT_DIRECTORY)
      cmake_path(ABSOLUTE_PATH arg_OUTPUT_DIRECTORY BASE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" NORMALIZE OUTPUT_VARIABLE dstDir)
      cmake_path(APPEND dstDir "icons")
      set(itemDst "")
      foreach(item IN LISTS arg_ICONS)
        list(APPEND itemDst "${dstDir}/${item}")
      endforeach()
      wxfb_copy_target_resources("${arg_TARGET}" "icons" "${itemSrc}" "${itemDst}")
    endif()
    if(DEFINED arg_INSTALL_DIRECTORY)
      wxfb_install_files("${srcDir}" "${itemSrc}" "${arg_INSTALL_DIRECTORY}/icons")
    endif()
  endif()
endfunction()


function(wxfb_copy_target_resources arg_TARGET arg_NAME arg_SOURCE_ITEMS arg_DESTINATION_ITEMS)
  add_custom_command(OUTPUT ${arg_DESTINATION_ITEMS} COMMENT "${arg_TARGET}: Copying ${arg_NAME}")
  foreach(src dst IN ZIP_LISTS arg_SOURCE_ITEMS arg_DESTINATION_ITEMS)
    add_custom_command(
      OUTPUT ${arg_DESTINATION_ITEMS}
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${src}" "${dst}"
      DEPENDS "${src}"
      VERBATIM
      APPEND
    )
  endforeach()
  add_custom_target(${arg_TARGET}-${arg_NAME} DEPENDS ${arg_DESTINATION_ITEMS})
  add_dependencies(${arg_TARGET} ${arg_TARGET}-${arg_NAME})
endfunction()


function(wxfb_install_files arg_BASE_DIRECTORY arg_FILES arg_DESTINATION)
  foreach(file IN LISTS arg_FILES)
    cmake_path(RELATIVE_PATH file BASE_DIRECTORY "${arg_BASE_DIRECTORY}" OUTPUT_VARIABLE fileBase)
    cmake_path(GET fileBase ROOT_PATH fileDir)
    install(FILES "${file}" DESTINATION "${arg_DESTINATION}/${fileDir}")
  endforeach()
endfunction()
