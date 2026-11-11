cmake_minimum_required(VERSION 3.21...3.31)

#[[
Utility functions to create self-contained, relocatable macOS application bundles.

The libraries the bundle depends on must be installed into the Contents/Frameworks directory of the bundle by
the project, e.g. with install(RUNTIME_DEPENDENCY_SET). This file takes care of what that command does not do:
Pointing all references to those libraries, including the references between the libraries themselves, to the
copies inside the bundle and signing the modified files again.

macos_bundle_install_fixup(<bundle> [COMPONENT <component>])

Add an installation step which processes the installed <bundle>. It must be called after all commands that
install files into the bundle, because the installation steps are executed in the order of their definition.
<bundle> is either an absolute path or a path relative to CMAKE_INSTALL_PREFIX. DESTDIR is taken into account.

macos_bundle_fixup(<bundle>)

Perform the processing directly, <bundle> must be an absolute path. This function can be used in scripts and is
what macos_bundle_install_fixup calls at install time.

All Mach-O files inside of Contents/MacOS, Contents/PlugIns and Contents/Frameworks are processed. A reference to
a library which is not located in a system directory is replaced by a @loader_path relative reference if a file
with the same name exists in Contents/Frameworks, so no RPATH is required to find them. Afterwards it is verified
that no reference points outside of the bundle. Since the modifications invalidate the code signatures, the bundle
is signed again with an ad-hoc signature.
]]
function(macos_bundle_install_fixup bundle)
  set(options "")
  set(singleValues COMPONENT)
  set(multiValues "")
  cmake_parse_arguments(arg "${options}" "${singleValues}" "${multiValues}" ${ARGN})

  if(NOT IS_ABSOLUTE "${bundle}")
    set(bundle "\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${bundle}")
  endif()

  set(componentArgument "")
  if(DEFINED arg_COMPONENT)
    set(componentArgument COMPONENT "${arg_COMPONENT}")
  endif()

  install(
    CODE
    "
      include(\"${CMAKE_CURRENT_FUNCTION_LIST_FILE}\")
      macos_bundle_fixup(\"${bundle}\")
    "
    ${componentArgument}
  )
endfunction()

function(macos_bundle_fixup bundle)
  find_program(file_cmd NAMES file REQUIRED)
  find_program(otool_cmd NAMES otool REQUIRED)
  find_program(install_name_tool_cmd NAMES install_name_tool REQUIRED)
  find_program(codesign_cmd NAMES codesign REQUIRED)

  set(frameworkDir "${bundle}/Contents/Frameworks")
  if(NOT IS_DIRECTORY "${bundle}/Contents")
    message(FATAL_ERROR "Not an application bundle: ${bundle}")
  endif()

  file(GLOB_RECURSE candidates LIST_DIRECTORIES false
    "${bundle}/Contents/MacOS/*"
    "${bundle}/Contents/PlugIns/*"
    "${bundle}/Contents/Frameworks/*"
  )
  set(binaries "")
  foreach(candidate IN LISTS candidates)
    if(IS_SYMLINK "${candidate}")
      continue()
    endif()
    # Mach-O (including universal) binaries; static archives and other files are skipped
    execute_process(
      COMMAND "${file_cmd}" --brief --mime-type "${candidate}"
      OUTPUT_VARIABLE mimeType
      OUTPUT_STRIP_TRAILING_WHITESPACE
      COMMAND_ERROR_IS_FATAL ANY
    )
    if(mimeType STREQUAL "application/x-mach-binary")
      list(APPEND binaries "${candidate}")
    endif()
  endforeach()

  # The files must be writable to be modified, the origin of the files might have installed them read-only
  foreach(binary IN LISTS binaries)
    file(CHMOD "${binary}" PERMISSIONS
      OWNER_READ OWNER_WRITE OWNER_EXECUTE
      GROUP_READ GROUP_EXECUTE
      WORLD_READ WORLD_EXECUTE
    )
  endforeach()

  foreach(binary IN LISTS binaries)
    cmake_path(GET binary PARENT_PATH binaryDir)
    cmake_path(RELATIVE_PATH frameworkDir BASE_DIRECTORY "${binaryDir}" OUTPUT_VARIABLE relativeFrameworkDir)
    if(relativeFrameworkDir STREQUAL ".")
      set(relativeFrameworkDir "")
    else()
      string(APPEND relativeFrameworkDir "/")
    endif()

    set(arguments "")
    cmake_path(GET binary FILENAME binaryName)
    if(binaryDir STREQUAL frameworkDir AND binaryName MATCHES "\\.dylib$")
      list(APPEND arguments -id "@rpath/${binaryName}")
    endif()

    _macos_bundle_get_references("${binary}" references)
    foreach(reference IN LISTS references)
      if(reference MATCHES "^(/usr/lib/|/System/Library/)")
        continue()
      endif()
      cmake_path(GET reference FILENAME referenceName)
      # The existence check also covers the symlinks that are installed along the libraries
      if(EXISTS "${frameworkDir}/${referenceName}")
        set(newReference "@loader_path/${relativeFrameworkDir}${referenceName}")
        if(NOT reference STREQUAL newReference)
          list(APPEND arguments -change "${reference}" "${newReference}")
        endif()
      endif()
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

  foreach(binary IN LISTS binaries)
    cmake_path(GET binary PARENT_PATH binaryDir)
    _macos_bundle_get_references("${binary}" references)
    foreach(reference IN LISTS references)
      if(reference MATCHES "^(/usr/lib/|/System/Library/)")
        continue()
      endif()
      if(reference MATCHES "^@loader_path/(.+)$" AND EXISTS "${binaryDir}/${CMAKE_MATCH_1}")
        continue()
      endif()
      message(FATAL_ERROR "${binary} references a library that is not part of the bundle: ${reference}")
    endforeach()
  endforeach()

  # The signing must happen from the inside to the outside, the bundle itself must be signed last
  # because its signature includes the signatures of its contents.
  foreach(item IN LISTS binaries bundle)
    execute_process(
      COMMAND "${codesign_cmd}" --force --sign - "${item}"
      RESULT_VARIABLE result
      ERROR_VARIABLE errorOutput
    )
    if(NOT result EQUAL 0)
      message(FATAL_ERROR "Unable to sign ${item}:\n${errorOutput}")
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

# Determine the libraries a Mach-O file references, without its own identification
function(_macos_bundle_get_references binary outputVar)
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

  # Libraries list their own identification like a dependency, modules have none
  execute_process(
    COMMAND "${otool_cmd}" -D "${binary}"
    OUTPUT_VARIABLE output
    RESULT_VARIABLE result
    ERROR_VARIABLE errorOutput
  )
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Unable to read the identification of ${binary}:\n${errorOutput}")
  endif()
  string(REGEX MATCH "\n([^\n]+)\n?$" match "${output}")
  if(match)
    list(REMOVE_ITEM references "${CMAKE_MATCH_1}")
  endif()
  set(${outputVar} ${references} PARENT_SCOPE)
endfunction()
