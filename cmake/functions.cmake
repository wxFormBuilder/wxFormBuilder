cmake_minimum_required(VERSION 3.21)

#[[
Add a wxFormBuilder plugin target.

wxfb_add_plugin(<name>
                [DIRECTORY <directory>]
                [SOURCES <source>...]
                [DEFINITIONS <define>...]
                [LIBRARIES <lib>...]
                [COMPONENTS <component>...]
                [ICONS <icon>...])

Adds a plugin target with the name wxFormBuilder_<name>. If <directory> is not specified the sources must reside
in a subdirectory relative to ${CMAKE_CURRENT_SOURCE_DIR} with the name <name>, otherwise with the name <directory>,
this is the root directory of the plugin.

The plugin interface implementation must reside in the file <name>.cpp inside the root directory. Additional
source files can be specified with SOURCES, the paths must be relative to the root directory. Additional defines and
libraries can be specified with DEFINITIONS and LIBRARIES.

If no COMPONENTS are specified a single component <name> must exist (XML definition file and code templates) in the
xml directory inside the root directory, otherwise these files must exist for every component.
Icon resources are specified with ICONS, the paths must be relative to the icon directory inside the root directory.
]]
function(wxfb_add_plugin PLUGIN_NAME)
  set(options "")
  set(singleValues DIRECTORY)
  set(multiValues SOURCES DEFINITIONS LIBRARIES COMPONENTS ICONS)
  cmake_parse_arguments(PLUGIN "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  if(NOT DEFINED PLUGIN_DIRECTORY)
    set(PLUGIN_DIRECTORY ${PLUGIN_NAME})
  endif()
  if(NOT DEFINED PLUGIN_COMPONENTS)
    set(PLUGIN_COMPONENTS ${PLUGIN_NAME})
  endif()
  if(DEFINED PLUGIN_SOURCES)
    list(TRANSFORM PLUGIN_SOURCES PREPEND ${PLUGIN_DIRECTORY}/)
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
    set(stageResourceDestination "${WXFB_STAGE_DIR}/${resourceDestination}")
  else()
    set(stageResourceDestination "")
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

  set_target_properties(wxFormBuilder_${PLUGIN_NAME} PROPERTIES FOLDER "Plugins/${PLUGIN_NAME}")

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
  wxfb_target_definitions(wxFormBuilder_${PLUGIN_NAME}
    INPUT_DIRECTORY "../output/plugins/${PLUGIN_DIRECTORY}"
    OUTPUT_DIRECTORY "${stageResourceDestination}"
    INSTALL_DIRECTORY "${resourceDestination}"
    COMMON ${PLUGIN_COMPONENTS}
    TEMPLATES ${PLUGIN_COMPONENTS}
  )
  set_target_properties(wxFormBuilder_${PLUGIN_NAME}-definitions wxFormBuilder_${PLUGIN_NAME}-templates PROPERTIES FOLDER "Plugins/${PLUGIN_NAME}")

  if(DEFINED PLUGIN_ICONS)
    wxfb_target_resources(wxFormBuilder_${PLUGIN_NAME}
      INPUT_DIRECTORY "../output/plugins/${PLUGIN_DIRECTORY}"
      OUTPUT_DIRECTORY "${stageResourceDestination}"
      INSTALL_DIRECTORY "${resourceDestination}"
      ICONS ${PLUGIN_ICONS}
    )
    set_target_properties(wxFormBuilder_${PLUGIN_NAME}-icons PROPERTIES FOLDER "Plugins/${PLUGIN_NAME}")
  endif()
endfunction()


#[[
Helper function to add XML definition files and code templates to a target with dependency tracking.

wxfb_target_definitions(<target>
                        INPUT_DIRECTORY <input-dir>
                        [OUTPUT_DIRECTORY <output-dir>]
                        [INSTALL_DIRECTORY <install-dir>]
                        [COMMON <common>...]
                        [TEMPLATES <template>...])

This function must be called at most one time for <target>.

Source files from <input-dir> are copied during the build phase into <output-dir> and installed into <install-dir>
during the install phase. The specification of <output-dir> and <install-dir> is optional, if not given the corresponding
steps are skipped.

If <input-dir> is not absolute it is interpreted relative to ${CMAKE_CURRENT_SOURCE_DIR}, if <output-dir> is not absolute
it is interpreted relative to ${CMAKE_CURRENT_BINARY_DIR}.

Simple definition files are specified with COMMON, code templates are specified with TEMPLATES.
]]
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


#[[
Helper function to add resources and icons to a target with dependency tracking.

wxfb_target_resources(<target>
                      INPUT_DIRECTORY <input-dir>
                      [OUTPUT_DIRECTORY <output-dir>]
                      [INSTALL_DIRECTORY <install-dir>]
                      [RESOURCES <resource>...]
                      [ICONS <icon>...])

This function must be called at most one time for <target>.

Source files from <input-dir> are copied during the build phase into <output-dir> and installed into <install-dir>
during the install phase. The specification of <output-dir> and <install-dir> is optional, if not given the corresponding
steps are skipped.

If <input-dir> is not absolute it is interpreted relative to ${CMAKE_CURRENT_SOURCE_DIR}, if <output-dir> is not absolute
it is interpreted relative to ${CMAKE_CURRENT_BINARY_DIR}.

Simple resources are specified with RESOURCES, icons are specified with ICONS. Icons must reside in the icon subdirectory
of <input-dir>.
]]
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


#[[
Helper function to copy files from source tree into binary tree with dependency tracking.

wxfb_copy_target_resources(<target> <name> <source-files> <destination-files>)

This function must be called at most one time for <target>.

During the build phase all <source-files> files are copied to <destination-files>. Both lists must contain files with
absolute paths and must be matching, each source entry is copied to the corresponding destination entry.
A target <target>-<name> is added to the default build target to track updates of <source-files>.
]]
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
  add_custom_target(${arg_TARGET}-${arg_NAME} ALL DEPENDS ${arg_DESTINATION_ITEMS})
endfunction()


#[[
Helper function to install files from source tree into installation directory.

wxfb_install_files(<base-directory> <files> <destination-directory>)

During the install phase all <files> are copied into <destination-directory> preserving directory structure.
The items of <files> are made relative to <base-directory>, the remaining directory structure
is created inside <destination-directory>.
]]
function(wxfb_install_files arg_BASE_DIRECTORY arg_FILES arg_DESTINATION)
  foreach(file IN LISTS arg_FILES)
    cmake_path(RELATIVE_PATH file BASE_DIRECTORY "${arg_BASE_DIRECTORY}" OUTPUT_VARIABLE fileBase)
    cmake_path(GET fileBase ROOT_PATH fileDir)
    install(FILES "${file}" DESTINATION "${arg_DESTINATION}/${fileDir}")
  endforeach()
endfunction()
