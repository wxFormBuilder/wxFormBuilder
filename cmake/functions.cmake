cmake_minimum_required(VERSION 3.21)

#[[
Set default build type.

wxfb_set_default_build_type([BUILD_TYPE <build-type>])

If the build type has not been set yet and this function is beeing called from a top level project or plugin host
for a single-config generator, sets CMAKE_BUILD_TYPE to <build-type>, otherwise it does nothing.
If <build-type> is not specified, Debug is used.
]]
function(wxfb_set_default_build_type)
  set(options "")
  set(singleValues BUILD_TYPE)
  set(multiValues "")
  cmake_parse_arguments(arg "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  set(buildTypes "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
  if(NOT DEFINED arg_BUILD_TYPE)
    set(arg_BUILD_TYPE "Debug")
  else()
    if(NOT arg_BUILD_TYPE IN_LIST buildTypes)
      message(FATAL_ERROR "Invalid build type: '${arg_BUILD_TYPE}'")
    endif()
  endif()

  if(PROJECT_IS_TOP_LEVEL OR WXFB_BUILD_PLUGIN_HOST)
    get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(NOT isMultiConfig)
      if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "Setting build type to '${arg_BUILD_TYPE}' as none was specified.")
        set(CMAKE_BUILD_TYPE "${arg_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${buildTypes}")
      elseif(NOT CMAKE_BUILD_TYPE IN_LIST buildTypes)
        message(FATAL_ERROR "Unknown build type: '${CMAKE_BUILD_TYPE}'")
      endif()
    endif()
  endif()
endfunction()

#[[
Set default compiler options.

wxfb_set_default_compiler_options([SKIP_SOURCE_CHARSET]
                                  [SKIP_WARNING_LEVEL])

This function should be called from the top level project after specifying the default build type
but before creating any targets that get compiled.

If SKIP_SOURCE_CHARSET is specified, the source charset value UTF-8 is not set.
If SKIP_WARNING_LEVEL is specified, the warning level is not set.
]]
function(wxfb_set_default_compiler_options)
  set(options SKIP_SOURCE_CHARSET SKIP_WARNING_LEVEL)
  set(singleValues "")
  set(multiValues "")
  cmake_parse_arguments(arg "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  if(NOT arg_SKIP_WARNING_LEVEL)
    wxfb_set_default_compiler_warnings()
    # FIXME: Internal knowledge that this variable must be passed upwards
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
  endif()

  if(MSVC)
    # Disable warnings about standard conformant code that is not conform to Microsoft standards
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS _SCL_SECURE_NO_WARNINGS)
    if(NOT arg_SKIP_SOURCE_CHARSET)
      # Without BOM the UTF-8 encoding doesn't get detected so enforce it
      add_compile_options(/source-charset:utf-8)
    endif()
    # Report the correct C++ version if supported
    if(MSVC_VERSION GREATER_EQUAL 1914)
      add_compile_options(/Zc:__cplusplus)
    endif()
  endif()
endfunction()

#[[
Set default compiler warning level.

wxfb_set_default_compiler_warnings()

This function should be called from the top level project after specifying the default build type
but before creating any targets that get compiled.
]]
function(wxfb_set_default_compiler_warnings)
  if(PROJECT_IS_TOP_LEVEL)
    # TODO: This is not the ideal solution to apply these warning flags.
    #       Toolchain files are not really intended for this purpose.
    #       Presets would be a viable solution, but currently, at least on VSCode, this
    #       has the side effect that pretty much everything from generator to build type
    #       to binary directory needs to be specified. The usual control ability of especially
    #       the build type gets lost in preset mode.
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
      string(APPEND CMAKE_CXX_FLAGS " /W4")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      # This assumes the default Clang frontend and not the MSVC compatible one
      string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra -Wpedantic")
    else()
      # This assumes GCC
      string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra -Wpedantic")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
  endif()
endfunction()

#[[
Set or verify language standards.

wxfb_set_language_standard([C_STANDARD <c-standard>]
                           [CXX_STANDARD <cxx-standard>]
                           [NO_POSITION_INDEPENDENT_CODE])

Sets the requested language standard if not already set, otherwise verifies that the set language standard
it not older than requested. If NO_POSITION_INDEPENDENT_CODE is not specified, position independent code gets enabled.
]]
function(wxfb_set_language_standard)
  set(options NO_POSITION_INDEPENDENT_CODE)
  set(singleValues C_STANDARD CXX_STANDARD)
  set(multiValues "")
  cmake_parse_arguments(arg "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  if(DEFINED arg_C_STANDARD)
    if(NOT DEFINED CMAKE_C_STANDARD)
      set(CMAKE_C_STANDARD ${arg_C_STANDARD} PARENT_SCOPE)
      message(STATUS "Setting CMAKE_C_STANDARD: ${arg_C_STANDARD}")
    elseif(NOT (
      (arg_C_STANDARD LESS 90 AND (CMAKE_C_STANDARD GREATER_EQUAL arg_C_STANDARD AND CMAKE_C_STANDARD LESS 90)) OR
      (arg_C_STANDARD GREATER_EQUAL 90 AND (CMAKE_C_STANDARD GREATER_EQUAL arg_C_STANDARD OR CMAKE_C_STANDARD LESS 90))
    ))
      message(FATAL_ERROR "The CMAKE_C_STANDARD value needs to be at least ${arg_C_STANDARD}, current value: ${CMAKE_C_STANDARD}")
    endif()
    set(CMAKE_C_STANDARD_REQUIRED ON PARENT_SCOPE)
    if(NOT DEFINED CMAKE_C_EXTENSIONS)
      set(CMAKE_C_EXTENSIONS OFF PARENT_SCOPE)
    endif()
  endif()

  if(DEFINED arg_CXX_STANDARD)
    if(NOT DEFINED CMAKE_CXX_STANDARD)
      set(CMAKE_CXX_STANDARD ${arg_CXX_STANDARD} PARENT_SCOPE)
      message(STATUS "Setting CMAKE_CXX_STANDARD: ${arg_CXX_STANDARD}")
    elseif(NOT (
      (arg_CXX_STANDARD LESS 98 AND (CMAKE_CXX_STANDARD GREATER_EQUAL arg_CXX_STANDARD AND CMAKE_CXX_STANDARD LESS 98)) OR
      (arg_CXX_STANDARD GREATER_EQUAL 98 AND (CMAKE_CXX_STANDARD GREATER_EQUAL arg_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 98))
    ))
      message(FATAL_ERROR "The CMAKE_CXX_STANDARD value needs to be at least ${arg_CXX_STANDARD}, current value: ${CMAKE_CXX_STANDARD}")
    endif()
    set(CMAKE_CXX_STANDARD_REQUIRED ON PARENT_SCOPE)
    if(NOT DEFINED CMAKE_CXX_EXTENSIONS)
      set(CMAKE_CXX_EXTENSIONS OFF PARENT_SCOPE)
    endif()
  endif()

  if(NOT arg_NO_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE TRUE PARENT_SCOPE)
    message(STATUS "Setting CMAKE_POSITION_INDEPENDENT_CODE: TRUE")
  endif()
endfunction()


#[[
Create a target for the wxWidgets library.

wxfb_create_target_wxwidgets([COMPONENTS <component>...]
                             [DISABLE_GUI])

This function must be called after the find_package() call of the wxWidgets library,
do not use the include(${wxWidgets_USE_FILE}) command when using this function.

The wxWidgets Find Module does not create a target yet but relies on variables,
this helper function does create a target from the search results of the Find Module.
Due to the nature of the search results, instead of an imported target, an interface target
gets created. This has the drawback that this target will be always in global scope,
so there can be only one library configuration present in a project.

In case wxWidgets was found by using config mode, the created targets are used instead. To prevent
the usage of every present target, especially unused targets that require external libraries,
the required components can be specified by <component>. The given components should be identical
to the components given to the find_package() call.

If DISABLE_GUI is specified, defines that disable GUI components get set. This allows to use the monolithic
variant of wxWidgets in Console Mode.
]]
function(wxfb_create_target_wxwidgets)
  set(options DISABLE_GUI)
  set(singleValues "")
  set(multiValues COMPONENTS)
  cmake_parse_arguments(arg "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  if(NOT arg_COMPONENTS)
    # If no components are specified, try to use all present ones
    set(arg_COMPONENTS base net core adv aui html propgrid ribbon richtext webview stc xrc media gl qa xml mono)
  endif()

  if(NOT wxWidgets_FOUND)
    message(FATAL_ERROR "wxWidgets NOT FOUND")
  endif()
  if(NOT wxWidgets_wxrc_EXECUTABLE)
    # Config-Mode does not search for wxrc, assume the environment is properly setup and search for it ourself
    find_program(wxWidgets_wxrc_EXECUTABLE NAMES wxrc REQUIRED)
    mark_as_advanced(wxWidgets_wxrc_EXECUTABLE)
  endif()

  if(NOT TARGET wxWidgets::wxWidgets)
    message(DEBUG "Creating target wxWidgets::wxWidgets")
    add_library(wxWidgets_wxWidgets INTERFACE)
    add_library(wxWidgets::wxWidgets ALIAS wxWidgets_wxWidgets)

    # To detect if CONFIG mode or MODULE mode was used, test for the presence of all required components
    set(targets "")
    foreach(component IN LISTS arg_COMPONENTS)
      message(DEBUG "Checking component: ${component}")
      if(TARGET wx::${component})
        message(DEBUG "Found target: wx::${component}")
        list(APPEND targets wx::${component})
      endif()
    endforeach()

    list(LENGTH targets targetCount)
    if(targetCount GREATER 0)
      message(DEBUG "wxWidgets targets: ${targets}")
      target_link_libraries(wxWidgets_wxWidgets INTERFACE ${targets})
    else()
      message(DEBUG "wxWidgets_INCLUDE_DIRS: ${wxWidgets_INCLUDE_DIRS}")
      if(wxWidgets_INCLUDE_DIRS)
        if(wxWidgets_INCLUDE_DIRS_NO_SYSTEM)
          target_include_directories(wxWidgets_wxWidgets INTERFACE ${wxWidgets_INCLUDE_DIRS})
        else()
          target_include_directories(wxWidgets_wxWidgets SYSTEM INTERFACE ${wxWidgets_INCLUDE_DIRS})
        endif()
      endif()
      message(DEBUG "wxWidgets_LIBRARY_DIRS: ${wxWidgets_LIBRARY_DIRS}")
      if(wxWidgets_LIBRARY_DIRS)
        target_link_directories(wxWidgets_wxWidgets INTERFACE ${wxWidgets_LIBRARY_DIRS})
      endif()
      message(DEBUG "wxWidgets_DEFINITIONS: ${wxWidgets_DEFINITIONS}")
      if(wxWidgets_DEFINITIONS)
        target_compile_definitions(wxWidgets_wxWidgets INTERFACE ${wxWidgets_DEFINITIONS})
      endif()
      message(DEBUG "wxWidgets_DEFINITIONS_DEBUG: ${wxWidgets_DEFINITIONS_DEBUG}")
      if(wxWidgets_DEFINITIONS_DEBUG)
        list(TRANSFORM wxWidgets_DEFINITIONS_DEBUG REPLACE [[^(.+)$]] [[$<$<CONFIG:Debug>:\1>]] OUTPUT_VARIABLE wxWidgets_DEFINITIONS_DEBUG_GENEX)
        message(DEBUG "wxWidgets_DEFINITIONS_DEBUG_GENEX: ${wxWidgets_DEFINITIONS_DEBUG_GENEX}")
        target_compile_definitions(wxWidgets_wxWidgets INTERFACE ${wxWidgets_DEFINITIONS_DEBUG_GENEX})
        unset(wxWidgets_DEFINITIONS_DEBUG_GENEX)
      endif()
      message(DEBUG "wxWidgets_CXX_FLAGS: ${wxWidgets_CXX_FLAGS}")
      if(wxWidgets_CXX_FLAGS)
        target_compile_options(wxWidgets_wxWidgets INTERFACE ${wxWidgets_CXX_FLAGS})
      endif()
      message(DEBUG "wxWidgets_LIBRARIES: ${wxWidgets_LIBRARIES}")
      if(wxWidgets_LIBRARIES)
        target_link_libraries(wxWidgets_wxWidgets INTERFACE ${wxWidgets_LIBRARIES})
      endif()
    endif()

    if(arg_DISABLE_GUI)
      target_compile_definitions(wxWidgets_wxWidgets INTERFACE wxUSE_GUI=0)
    endif()
  endif()

  if(NOT TARGET wxWidgets::wxrc)
    message(DEBUG "Creating target wxWidgets::wxrc")
    add_executable(wxWidgets::wxrc IMPORTED GLOBAL)
    set_target_properties(wxWidgets::wxrc PROPERTIES IMPORTED_LOCATION "${wxWidgets_wxrc_EXECUTABLE}")
  endif()
endfunction()


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
  set(sourceFiles "")
  set(binaryFiles "")
  foreach(file IN LISTS fileSources)
    cmake_path(IS_PREFIX sourceDir "${file}" isSourceFile)
    cmake_path(IS_PREFIX binaryDir "${file}" isBinaryFile)
    if(isBinaryFile)
      list(APPEND binaryFiles "${file}")
    elseif(isSourceFile)
      list(APPEND sourceFiles "${file}")
    endif()
  endforeach()

  set(filterSources ${sourceFiles})
  list(FILTER filterSources INCLUDE REGEX "/.+\\.h(h|pp)?$")
  source_group(
    TREE "${sourceTreeDir}"
    PREFIX "Header Files"
    FILES ${filterSources}
  )
  set(filterSources ${sourceFiles})
  list(FILTER filterSources INCLUDE REGEX "/.+\\.h(h|pp)?\\.in$")
  source_group(
    TREE "${sourceTreeDir}"
    PREFIX "Header Templates"
    FILES ${filterSources}
  )
  set(filterSources ${sourceFiles})
  list(FILTER filterSources INCLUDE REGEX "/.+\\.c(c|xx|pp)?$")
  source_group(
    TREE "${sourceTreeDir}"
    PREFIX "Source Files"
    FILES ${filterSources}
  )
  set(filterSources ${sourceFiles})
  list(FILTER filterSources INCLUDE REGEX "/.+\\.c(c|xx|pp)?\\.in$")
  source_group(
    TREE "${sourceTreeDir}"
    PREFIX "Source Templates"
    FILES ${filterSources}
  )

  set(filterSources ${binaryFiles})
  source_group(
    TREE "${binaryTreeDir}"
    PREFIX "Generated Files"
    FILES ${filterSources}
  )
  if(DEFINED arg_EXTRA_BINARY_DIRECTORY)
    # If the specified directory contains the binary dir of the target, the files will get added
    # twice with a different path, so exclude that directory.
    set(filterSources "")
    foreach(file IN LISTS fileSources)
      cmake_path(IS_PREFIX arg_EXTRA_BINARY_DIRECTORY "${file}" isExtraBinaryFile)
      if(isExtraBinaryFile)
        cmake_path(IS_PREFIX binaryDir "${file}" isBinaryFile)
        if(NOT isBinaryFile)
          list(APPEND filterSources "${file}")
        endif()
      endif()
    endforeach()
    source_group(
      TREE "${arg_EXTRA_BINARY_DIRECTORY}"
      PREFIX "Generated Files"
      FILES ${filterSources}
    )
  endif()
endfunction()
