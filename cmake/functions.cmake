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
    list(TRANSFORM PLUGIN_SOURCES PREPEND "${PLUGIN_DIRECTORY}/")
  endif()

  if(WIN32)
    set(runtimeDestination ${WXFB_INSTALL_BINDIR}/plugins/${PLUGIN_NAME})
    set(libraryDestination ${WXFB_INSTALL_LIBDIR}/plugins/${PLUGIN_NAME})
    set(archiveDestination ${WXFB_INSTALL_LIBDIR}/plugins/${PLUGIN_NAME})
  elseif(APPLE)
    set(runtimeDestination wxFormBuilder.app/Contents/PlugIns)
    set(libraryDestination wxFormBuilder.app/Contents/PlugIns)
    set(archiveDestination wxFormBuilder.app/Contents/PlugIns)
  else()
    set(runtimeDestination ${WXFB_INSTALL_BINDIR}/wxformbuilder)
    set(libraryDestination ${WXFB_INSTALL_LIBDIR}/wxformbuilder)
    set(archiveDestination ${WXFB_INSTALL_LIBDIR}/wxformbuilder)
  endif()
  set(resourceDestination ${WXFB_INSTALL_DATADIR}/plugins/${PLUGIN_NAME})

  # TODO: Is this required or does MODULE always set this property?
  set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
  if(WXFB_STAGE_BUILD)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
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
    OUTPUT_NAME "${PLUGIN_NAME}"
  )
  if(APPLE)
    # The current plugin loader code requires this extension
    # TODO: Setting CMAKE_SHARED_MODULE_SUFFIX inside this function has no effect, setting it
    #       in the toplevel CMake file has. Setting the property directly does also work.
    set_target_properties(wxFormBuilder_${PLUGIN_NAME} PROPERTIES SUFFIX ".dylib")
  endif()

  target_sources(wxFormBuilder_${PLUGIN_NAME} PRIVATE "${PLUGIN_DIRECTORY}/${PLUGIN_NAME}.cpp")
  if(DEFINED PLUGIN_SOURCES)
    target_sources(wxFormBuilder_${PLUGIN_NAME} PRIVATE ${PLUGIN_SOURCES})
  endif()

  if(DEFINED PLUGIN_DEFINITIONS)
    target_compile_definitions(wxFormBuilder_${PLUGIN_NAME} PRIVATE ${PLUGIN_DEFINITIONS})
  endif()

  target_link_libraries(wxFormBuilder_${PLUGIN_NAME} PUBLIC wxFormBuilder::plugin-interface)
  if(DEFINED PLUGIN_LIBRARIES)
    target_link_libraries(wxFormBuilder_${PLUGIN_NAME} PRIVATE ${PLUGIN_LIBRARIES})
  endif()

  if(APPLE)
    set_target_properties(wxFormBuilder_${PLUGIN_NAME} PROPERTIES INSTALL_RPATH "@loader_path/../Frameworks")
  else()
    set_target_properties(wxFormBuilder_${PLUGIN_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/..")
  endif()

  set_target_properties(wxFormBuilder_${PLUGIN_NAME} PROPERTIES FOLDER "Plugins/${PLUGIN_NAME}")
  wxfb_target_source_groups(wxFormBuilder_${PLUGIN_NAME} STRIP_PREFIX "${PLUGIN_DIRECTORY}")

  # Do not install the RUNTIME_DEPENDENCIES here, because of the currently defined install locations
  # they would get installed next to the plugin. Instead, register the dependencies in a RUNTIME_DEPENDENCY_SET
  # and let the main application install them. This way the dependencies get installed into the locations
  # defined by the main application.
  install(TARGETS wxFormBuilder_${PLUGIN_NAME}
    RUNTIME_DEPENDENCY_SET wxFormBuilder_dependencies
    RUNTIME
      DESTINATION ${runtimeDestination}
    LIBRARY
      DESTINATION ${libraryDestination}
  )

  wxfb_target_definitions(wxFormBuilder_${PLUGIN_NAME}
    INPUT_DIRECTORY "${PLUGIN_DIRECTORY}"
    OUTPUT_DIRECTORY "${stageResourceDestination}"
    INSTALL_DIRECTORY "${resourceDestination}"
    FOLDER "Plugins/${PLUGIN_NAME}"
    COMMON ${PLUGIN_COMPONENTS}
    TEMPLATES ${PLUGIN_COMPONENTS}
  )

  if(DEFINED PLUGIN_ICONS)
    wxfb_target_resources(wxFormBuilder_${PLUGIN_NAME}
      INPUT_DIRECTORY "${PLUGIN_DIRECTORY}"
      OUTPUT_DIRECTORY "${stageResourceDestination}"
      INSTALL_DIRECTORY "${resourceDestination}"
      FOLDER "Plugins/${PLUGIN_NAME}"
      ICONS ${PLUGIN_ICONS}
    )
  endif()
endfunction()


#[[
Helper function to add XML definition files and code templates to a target with dependency tracking.

wxfb_target_definitions(<target>
                        INPUT_DIRECTORY <input-dir>
                        [OUTPUT_DIRECTORY <output-dir>]
                        [INSTALL_DIRECTORY <install-dir>]
                        [FOLDER <folder>]
                        [COMMON <common>...]
                        [TEMPLATES <template>...])

This function must be called at most one time for <target>.

Source files from <input-dir> are copied during the build phase into <output-dir> and installed into <install-dir>
during the install phase. The specification of <output-dir> and <install-dir> is optional, if not given the corresponding
steps are skipped.

If <input-dir> is not absolute it is interpreted relative to ${CMAKE_CURRENT_SOURCE_DIR}, if <output-dir> is not absolute
it is interpreted relative to ${CMAKE_CURRENT_BINARY_DIR}.

If <folder> is specified, the created utility targets will be placed into that folder.

Simple definition files are specified with COMMON, code templates are specified with TEMPLATES.
]]
function(wxfb_target_definitions arg_TARGET)
  set(options "")
  set(singleValues INPUT_DIRECTORY OUTPUT_DIRECTORY INSTALL_DIRECTORY FOLDER)
  set(multiValues COMMON TEMPLATES)
  cmake_parse_arguments(arg "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  if(NOT DEFINED arg_INPUT_DIRECTORY)
    message(FATAL_ERROR "No input directory specified")
  endif()
  if(NOT DEFINED arg_FOLDER)
    set(arg_FOLDER "")
  endif()

  cmake_path(ABSOLUTE_PATH arg_INPUT_DIRECTORY BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE OUTPUT_VARIABLE inputDir)
  cmake_path(APPEND inputDir "xml")
  if(DEFINED arg_OUTPUT_DIRECTORY)
    cmake_path(ABSOLUTE_PATH arg_OUTPUT_DIRECTORY BASE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" NORMALIZE OUTPUT_VARIABLE outputDir)
    cmake_path(APPEND outputDir "xml")
  endif()

  if(DEFINED arg_COMMON)
    set(inputFiles "")
    foreach(file IN LISTS arg_COMMON)
      cmake_path(SET inputFile NORMALIZE "${inputDir}/${file}.xml")
      list(APPEND inputFiles "${inputFile}")
    endforeach()

    if(DEFINED arg_OUTPUT_DIRECTORY)
      set(outputFiles "")
      foreach(file IN LISTS arg_COMMON)
        cmake_path(SET outputFile NORMALIZE "${outputDir}/${file}.xml")
        list(APPEND outputFiles "${outputFile}")
      endforeach()
      wxfb_copy_target_resources("${arg_TARGET}" "common" "${arg_FOLDER}" "${inputFiles}" "${outputFiles}")
    endif()
    if(DEFINED arg_INSTALL_DIRECTORY)
      wxfb_install_files("${inputDir}" "${inputFiles}" "${arg_INSTALL_DIRECTORY}/xml")
    endif()
  endif()

  if(DEFINED arg_TEMPLATES)
    set(inputFiles "")
    foreach(file IN LISTS arg_TEMPLATES)
      foreach(lang IN LISTS WXFB_GENERATOR_LANGUAGES)
        cmake_path(SET inputFile NORMALIZE "${inputDir}/${file}.${lang}code")
        list(APPEND inputFiles "${inputFile}")
      endforeach()
    endforeach()

    if(DEFINED arg_OUTPUT_DIRECTORY)
      set(outputFiles "")
      foreach(file IN LISTS arg_TEMPLATES)
        foreach(lang IN LISTS WXFB_GENERATOR_LANGUAGES)
          cmake_path(SET outputFile NORMALIZE "${outputDir}/${file}.${lang}code")
          list(APPEND outputFiles "${outputFile}")
        endforeach()
      endforeach()
      wxfb_copy_target_resources("${arg_TARGET}" "templates" "${arg_FOLDER}" "${inputFiles}" "${outputFiles}")
    endif()
    if(DEFINED arg_INSTALL_DIRECTORY)
      wxfb_install_files("${inputDir}" "${inputFiles}" "${arg_INSTALL_DIRECTORY}/xml")
    endif()
  endif()
endfunction()


#[[
Helper function to add resources and icons to a target with dependency tracking.

wxfb_target_resources(<target>
                      INPUT_DIRECTORY <input-dir>
                      [OUTPUT_DIRECTORY <output-dir>]
                      [INSTALL_DIRECTORY <install-dir>]
                      [FOLDER <folder>]
                      [RESOURCES <resource>...]
                      [ICONS <icon>...])

This function must be called at most one time for <target>.

Source files from <input-dir> are copied during the build phase into <output-dir> and installed into <install-dir>
during the install phase. The specification of <output-dir> and <install-dir> is optional, if not given the corresponding
steps are skipped.

If <input-dir> is not absolute it is interpreted relative to ${CMAKE_CURRENT_SOURCE_DIR}, if <output-dir> is not absolute
it is interpreted relative to ${CMAKE_CURRENT_BINARY_DIR}.

If <folder> is specified, the created utility targets will be placed into that folder.

Simple resources are specified with RESOURCES, icons are specified with ICONS. Icons must reside in the icon subdirectory
of <input-dir>.
]]
function(wxfb_target_resources arg_TARGET)
  set(options "")
  set(singleValues INPUT_DIRECTORY OUTPUT_DIRECTORY INSTALL_DIRECTORY FOLDER)
  set(multiValues RESOURCES ICONS)
  cmake_parse_arguments(arg "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  if(NOT DEFINED arg_INPUT_DIRECTORY)
    message(FATAL_ERROR "No input directory specified")
  endif()
  if(NOT DEFINED arg_FOLDER)
    set(arg_FOLDER "")
  endif()

  if(DEFINED arg_RESOURCES)
    cmake_path(ABSOLUTE_PATH arg_INPUT_DIRECTORY BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE OUTPUT_VARIABLE inputDir)
    set(inputFiles "")
    foreach(file IN LISTS arg_RESOURCES)
      cmake_path(SET inputFile NORMALIZE "${inputDir}/${file}")
      list(APPEND inputFiles "${inputFile}")
    endforeach()

    if(DEFINED arg_OUTPUT_DIRECTORY)
      cmake_path(ABSOLUTE_PATH arg_OUTPUT_DIRECTORY BASE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" NORMALIZE OUTPUT_VARIABLE outputDir)
      set(outputFiles "")
      foreach(file IN LISTS arg_RESOURCES)
        cmake_path(SET outputFile NORMALIZE "${outputDir}/${file}")
        list(APPEND outputFiles "${outputFile}")
      endforeach()
      wxfb_copy_target_resources("${arg_TARGET}" "resources" "${arg_FOLDER}" "${inputFiles}" "${outputFiles}")
    endif()
    if(DEFINED arg_INSTALL_DIRECTORY)
      wxfb_install_files("${inputDir}" "${inputFiles}" "${arg_INSTALL_DIRECTORY}")
    endif()
  endif()

  if(DEFINED arg_ICONS)
    cmake_path(ABSOLUTE_PATH arg_INPUT_DIRECTORY BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE OUTPUT_VARIABLE inputDir)
    cmake_path(APPEND inputDir "icons")
    set(inputFiles "")
    foreach(file IN LISTS arg_ICONS)
      cmake_path(SET inputFile NORMALIZE "${inputDir}/${file}")
      list(APPEND inputFiles "${inputFile}")
    endforeach()

    if(DEFINED arg_OUTPUT_DIRECTORY)
      cmake_path(ABSOLUTE_PATH arg_OUTPUT_DIRECTORY BASE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" NORMALIZE OUTPUT_VARIABLE outputDir)
      cmake_path(APPEND outputDir "icons")
      set(outputFiles "")
      foreach(file IN LISTS arg_ICONS)
        cmake_path(SET outputFile NORMALIZE "${outputDir}/${file}")
        list(APPEND outputFiles "${outputFile}")
      endforeach()
      wxfb_copy_target_resources("${arg_TARGET}" "icons" "${arg_FOLDER}" "${inputFiles}" "${outputFiles}")
    endif()
    if(DEFINED arg_INSTALL_DIRECTORY)
      wxfb_install_files("${inputDir}" "${inputFiles}" "${arg_INSTALL_DIRECTORY}/icons")
    endif()
  endif()
endfunction()


#[[
Helper function to copy files from source tree into binary tree with dependency tracking.

wxfb_copy_target_resources(<target> <name> <folder> <source-files> <destination-files>)

This function must be called at most one time for <target>.

During the build phase all <source-files> files are copied to <destination-files>. Both lists must contain files with
absolute paths and must be matching, each source entry is copied to the corresponding destination entry.

A target <target>-<name> is added to the default build target to track updates of <source-files>.
If <folder> is not empty, the created target gets placed into that folder.
]]
function(wxfb_copy_target_resources arg_TARGET arg_NAME arg_FOLDER arg_SOURCE_ITEMS arg_DESTINATION_ITEMS)
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
  if(NOT arg_FOLDER STREQUAL "")
    set_target_properties(${arg_TARGET}-${arg_NAME} PROPERTIES FOLDER "${arg_FOLDER}")
  endif()
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
    cmake_path(GET fileBase PARENT_PATH fileDir)
    cmake_path(SET installDir NORMALIZE "${arg_DESTINATION}/${fileDir}")
    install(FILES "${file}" DESTINATION "${installDir}")
  endforeach()
endfunction()


#[[
Create default source groups.

wxfb_target_source_groups(<target>
                          [STRIP_PREFIX <strip-prefix>]
                          [STRIP_SOURCE_PREFIX <strip-source-prefix>]
                          [STRIP_BINARY_PREFIX <strip-binary-prefix>]
                          [EXTRA_BINARY_DIRECTORY <extra-binary-dir>])

Create source groups "Header Files", "Header Templates", "Source Files", "Source Templates",
"Generated Files" for the source files of the target <target>. Only files inside the directory
trees rooted at SOURCE_DIR and BINARY_DIR of <target> are grouped.

If specified, the common path prefix <strip-source-prefix> of the files inside SOURCE_DIR and
the common path prefix <strip-binary-prefix> of the files inside BINARY_DIR gets removed,
it is an error if not all paths start with that prefix. Use <strip-prefix> if the same prefix
should be used for SOURCE_DIR and BINARY_DIR, the other two parameters must not be specified in that case.

If <extra-binary-dir> is specified, source files of <target> inside that directory will also get added
to the group "Generated Files", no prefix stripping will be applied to these files. If <extra-binary-dir>
is not absolute it is interpreted relative to ${CMAKE_CURRENT_BINARY_DIR}. The <extra-binary-dir> may
contain BINARY_DIR of <target>, the contents of BINARY_DIR will get excluded when processing <extra-binary-dir>.
]]
function(wxfb_target_source_groups arg_TARGET)
  set(options "")
  set(singleValues STRIP_PREFIX STRIP_SOURCE_PREFIX STRIP_BINARY_PREFIX EXTRA_BINARY_DIRECTORY)
  set(multiValues "")
  cmake_parse_arguments(arg "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  get_target_property(sourceDir ${arg_TARGET} SOURCE_DIR)
  get_target_property(binaryDir ${arg_TARGET} BINARY_DIR)
  get_target_property(sources ${arg_TARGET} SOURCES)

  if(DEFINED arg_STRIP_PREFIX)
    if(DEFINED arg_STRIP_SOURCE_PREFIX OR DEFINED arg_STRIP_BINARY_PREFIX)
      message(FATAL_ERROR "STRIP_PREFIX must not be used together with STRIP_SOURCE_PREFIX or STRIP_BINARY_PREFIX")
    endif()
    set(arg_STRIP_SOURCE_PREFIX "${arg_STRIP_PREFIX}")
    set(arg_STRIP_BINARY_PREFIX "${arg_STRIP_PREFIX}")
  endif()
  if(DEFINED arg_STRIP_SOURCE_PREFIX)
    cmake_path(SET sourceTreeDir NORMALIZE "${sourceDir}/${arg_STRIP_SOURCE_PREFIX}")
  else()
    set(sourceTreeDir "${sourceDir}")
  endif()
  if(DEFINED arg_STRIP_BINARY_PREFIX)
    cmake_path(SET binaryTreeDir NORMALIZE "${binaryDir}/${arg_STRIP_BINARY_PREFIX}")
  else()
    set(binaryTreeDir "${binaryDir}")
  endif()
  if(DEFINED arg_EXTRA_BINARY_DIRECTORY)
    cmake_path(ABSOLUTE_PATH arg_EXTRA_BINARY_DIRECTORY BASE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" NORMALIZE)
  endif()

  set(fileSources "")
  foreach(source IN LISTS sources)
    cmake_path(ABSOLUTE_PATH source BASE_DIRECTORY "${sourceDir}" NORMALIZE OUTPUT_VARIABLE file)
    list(APPEND fileSources "${file}")
  endforeach()
  # Normally, the build tree is a subdirectory of the source tree. For the root directory the prefix match
  # will also match the binary tree, so create separate file lists by filtering the binary prefix explicit.
  # This will fail when doing an in-tree build, in that case everything will be classified as binary.
  set(sourceFiles ${fileSources})
  list(FILTER sourceFiles EXCLUDE REGEX "^${binaryDir}/")
  set(binaryFiles ${fileSources})
  list(FILTER binaryFiles INCLUDE REGEX "^${binaryDir}/")

  set(filterSources ${sourceFiles})
  list(FILTER filterSources INCLUDE REGEX "^${sourceDir}/.+\\.h(h|pp)?$")
  source_group(
    TREE "${sourceTreeDir}"
    PREFIX "Header Files"
    FILES ${filterSources}
  )
  set(filterSources ${sourceFiles})
  list(FILTER filterSources INCLUDE REGEX "^${sourceDir}/.+\\.h(h|pp)?\\.in$")
  source_group(
    TREE "${sourceTreeDir}"
    PREFIX "Header Templates"
    FILES ${filterSources}
  )
  set(filterSources ${sourceFiles})
  list(FILTER filterSources INCLUDE REGEX "^${sourceDir}/.+\\.c(c|xx|pp)?$")
  source_group(
    TREE "${sourceTreeDir}"
    PREFIX "Source Files"
    FILES ${filterSources}
  )
  set(filterSources ${sourceFiles})
  list(FILTER filterSources INCLUDE REGEX "^${sourceDir}/.+\\.c(c|xx|pp)?\\.in$")
  source_group(
    TREE "${sourceTreeDir}"
    PREFIX "Source Templates"
    FILES ${filterSources}
  )

  set(filterSources ${binaryFiles})
  list(FILTER filterSources INCLUDE REGEX "^${binaryDir}/")
  source_group(
    TREE "${binaryTreeDir}"
    PREFIX "Generated Files"
    FILES ${filterSources}
  )
  if(DEFINED arg_EXTRA_BINARY_DIRECTORY)
    # If the specified directory contains the binary dir of the target, the files will get added
    # twice with a different path, so exclude that directory.
    set(filterSources ${fileSources})
    list(FILTER filterSources INCLUDE REGEX "^${arg_EXTRA_BINARY_DIRECTORY}/")
    list(FILTER filterSources EXCLUDE REGEX "^${binaryDir}/")
    source_group(
      TREE "${arg_EXTRA_BINARY_DIRECTORY}"
      PREFIX "Generated Files"
      FILES ${filterSources}
    )
  endif()
endfunction()
