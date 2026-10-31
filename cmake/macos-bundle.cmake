cmake_minimum_required(VERSION 3.21...3.31)

#[[
Install-time helper to make a macOS application bundle self-contained.

This file is not included during configuration, it is included by the code of an install(CODE) command.

The required libraries of the application and its plugins are determined, copied into the standard
Contents/Frameworks directory of the bundle and all references to them, including the references between the
libraries themselves, are adjusted to point to the copies inside the bundle. Since the modification of the
Mach-O files invalidates their code signature, everything gets signed again with an ad-hoc signature.

wxfb_macos_bundle_fixup(<bundle>
  EXECUTABLE <name>
  [DIRECTORIES <directory>...]
)

<bundle> is the path to the bundle. The EXECUTABLE is the name of the main executable inside Contents/MacOS.
All *.dylib files inside Contents/PlugIns are treated as dynamically loaded modules.
Additional directories to search for libraries that are referenced via @rpath can be specified with DIRECTORIES.
]]
function(wxfb_macos_bundle_fixup bundle)
  set(options "")
  set(singleValues EXECUTABLE)
  set(multiValues DIRECTORIES)
  cmake_parse_arguments(arg "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  find_program(otool_cmd NAMES otool REQUIRED)
  find_program(install_name_tool_cmd NAMES install_name_tool REQUIRED)
  find_program(codesign_cmd NAMES codesign REQUIRED)

  set(executable "${bundle}/Contents/MacOS/${arg_EXECUTABLE}")
  set(pluginDir "${bundle}/Contents/PlugIns")
  set(frameworkDir "${bundle}/Contents/Frameworks")
  if(NOT EXISTS "${executable}")
    message(FATAL_ERROR "Bundle executable does not exist: ${executable}")
  endif()
  file(GLOB plugins LIST_DIRECTORIES false "${pluginDir}/*.dylib")

  # System libraries are present on every system and must not be bundled
  set(systemRegexes "^/usr/lib/" "^/System/Library/")

  file(GET_RUNTIME_DEPENDENCIES
    EXECUTABLES "${executable}"
    MODULES ${plugins}
    DIRECTORIES ${arg_DIRECTORIES}
    PRE_EXCLUDE_REGEXES ${systemRegexes}
    POST_EXCLUDE_REGEXES ${systemRegexes}
    RESOLVED_DEPENDENCIES_VAR resolvedDependencies
    UNRESOLVED_DEPENDENCIES_VAR unresolvedDependencies
    CONFLICTING_DEPENDENCIES_PREFIX conflictingDependencies
  )
  if(unresolvedDependencies)
    list(JOIN unresolvedDependencies "\n  " unresolvedList)
    message(FATAL_ERROR "Unable to resolve the following dependencies:\n  ${unresolvedList}")
  endif()
  if(conflictingDependencies_FILENAMES)
    list(JOIN conflictingDependencies_FILENAMES "\n  " conflictingList)
    message(FATAL_ERROR "Found different libraries with the same name:\n  ${conflictingList}")
  endif()

  # Libraries can be referenced via their real file name or via symlinks to it. Copy every library only
  # once under its real file name and map all possible reference names to it, otherwise the same library
  # could get loaded multiple times from different paths.
  file(MAKE_DIRECTORY "${frameworkDir}")
  set(referenceNames "")
  set(bundledNames "")
  set(bundledFiles "")
  foreach(dependency IN LISTS resolvedDependencies)
    # Skip libraries that are already inside the bundle
    string(FIND "${dependency}" "${bundle}/" position)
    if(position EQUAL 0)
      continue()
    endif()
    file(REAL_PATH "${dependency}" realPath)
    cmake_path(GET dependency FILENAME referenceName)
    cmake_path(GET realPath FILENAME bundledName)
    list(APPEND referenceNames "${referenceName}")
    list(APPEND bundledNames "${bundledName}")
    if(NOT bundledName IN_LIST bundledFiles)
      message(STATUS "Bundling: ${realPath}")
      file(COPY_FILE "${realPath}" "${frameworkDir}/${bundledName}" ONLY_IF_DIFFERENT)
      # The original files might be read-only, but they need to be modified
      file(CHMOD "${frameworkDir}/${bundledName}"
        PERMISSIONS
          OWNER_READ OWNER_WRITE OWNER_EXECUTE
          GROUP_READ GROUP_EXECUTE
          WORLD_READ WORLD_EXECUTE
      )
      list(APPEND bundledFiles "${bundledName}")
    endif()
  endforeach()

  # Libraries use paths relative to their own location, so they do not depend on the RPATH of the loading binary
  file(GLOB libraries LIST_DIRECTORIES false "${frameworkDir}/*.dylib")
  foreach(binary IN LISTS libraries plugins executable)
    if(binary IN_LIST libraries)
      set(isLibrary TRUE)
      set(referencePrefix "@loader_path")
      cmake_path(GET binary FILENAME binaryName)
      set(arguments -id "@rpath/${binaryName}")
    else()
      set(isLibrary FALSE)
      set(referencePrefix "@rpath")
      set(arguments "")
    endif()
    _wxfb_macos_bundle_get_references("${binary}" references ${isLibrary})
    foreach(reference IN LISTS references)
      if(reference MATCHES "^/usr/lib/" OR reference MATCHES "^/System/Library/")
        continue()
      endif()
      cmake_path(GET reference FILENAME referenceName)
      list(FIND referenceNames "${referenceName}" index)
      if(index EQUAL -1)
        continue()
      endif()
      list(GET bundledNames ${index} bundledName)
      list(APPEND arguments -change "${reference}" "${referencePrefix}/${bundledName}")
    endforeach()
    if(arguments)
      execute_process(
        COMMAND "${install_name_tool_cmd}" ${arguments} "${binary}"
        RESULT_VARIABLE result
        ERROR_VARIABLE errorOutput
      )
      if(NOT result EQUAL 0)
        message(FATAL_ERROR "Unable to adjust references of ${binary}:\n${errorOutput}")
      endif()
    endif()
  endforeach()

  # Verify that nothing references libraries outside of the bundle
  foreach(binary IN LISTS libraries plugins executable)
    if(binary IN_LIST libraries)
      set(isLibrary TRUE)
    else()
      set(isLibrary FALSE)
    endif()
    _wxfb_macos_bundle_get_references("${binary}" references ${isLibrary})
    foreach(reference IN LISTS references)
      if(reference MATCHES "^/usr/lib/" OR reference MATCHES "^/System/Library/")
        continue()
      endif()
      if(reference MATCHES "^@(rpath|loader_path)/(.+)$")
        set(referencedFile "${frameworkDir}/${CMAKE_MATCH_2}")
        if(EXISTS "${referencedFile}")
          continue()
        endif()
      endif()
      message(FATAL_ERROR "${binary} references a library outside of the bundle: ${reference}")
    endforeach()
  endforeach()

  # Modifying the files invalidated their signatures. The signing must happen from the inside to the outside,
  # the bundle itself must be signed last because its signature includes the signatures of its contents.
  foreach(binary IN LISTS libraries plugins executable bundle)
    execute_process(
      COMMAND "${codesign_cmd}" --force --sign - "${binary}"
      RESULT_VARIABLE result
      ERROR_VARIABLE errorOutput
    )
    if(NOT result EQUAL 0)
      message(FATAL_ERROR "Unable to sign ${binary}:\n${errorOutput}")
    endif()
  endforeach()
  execute_process(
    COMMAND "${codesign_cmd}" --verify --deep --strict --verbose=2 "${bundle}"
    RESULT_VARIABLE result
    ERROR_VARIABLE errorOutput
  )
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Signature verification of ${bundle} failed:\n${errorOutput}")
  endif()
endfunction()

# Determine the libraries a Mach-O file references, without its own identification if it is a library
function(_wxfb_macos_bundle_get_references binary outputVar isLibrary)
  execute_process(
    COMMAND "${otool_cmd}" -L "${binary}"
    OUTPUT_VARIABLE output
    RESULT_VARIABLE result
    ERROR_VARIABLE errorOutput
  )
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Unable to read the references of ${binary}:\n${errorOutput}")
  endif()

  string(REGEX MATCHALL "\n\t[^\n]+ \\(compatibility version" lines "${output}")
  set(references "")
  foreach(line IN LISTS lines)
    string(REGEX REPLACE "^\n\t(.+) \\(compatibility version$" "\\1" reference "${line}")
    list(APPEND references "${reference}")
  endforeach()

  # For libraries the first entry is the own identification, which is not a dependency
  if(isLibrary)
    list(POP_FRONT references)
  endif()
  set(${outputVar} ${references} PARENT_SCOPE)
endfunction()
