cmake_minimum_required(VERSION 3.20)

#[[
Configure CMake environment for building wxFormBuilder and third-party code.
]]
macro(wxfb_configure_prebuild)
  # Needs to be enabled for static libraries that get linked into shared libraries
  set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
  # Set own, adjusted, installation paths for third-party code
  set(CMAKE_INSTALL_BINDIR ${WXFB_INSTALL_BINDIR})
  set(CMAKE_INSTALL_LIBDIR ${WXFB_INSTALL_LIBDIR})
  set(CMAKE_INSTALL_DATADIR ${WXFB_INSTALL_DATADIR})
  # Use installation layout for staged build
  if(WXFB_STAGE_BUILD)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${WXFB_STAGE_DIR}/${CMAKE_INSTALL_BINDIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${WXFB_STAGE_DIR}/${CMAKE_INSTALL_LIBDIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${WXFB_STAGE_DIR}/${CMAKE_INSTALL_LIBDIR}")
  endif()
endmacro()

#[[
Perform post build configuration steps after building all wxFormBuilder components.
]]
macro(wxfb_configure_postbuild)
  if(APPLE)
    # TODO: Is there any possibility to determine the output filenames of the plugins to create a list that can be inserted literal?
    # TODO: The fixup_bundle command erases all present RPATH information. Since it fixes the loader paths of all libraries and none
    #       of them loads libraries dynamically, it is sufficient to add an RPATH only to the PlugIns directory which contains the
    #       libraries that get loaded dynamically.
    install(
      CODE
      "
        include(BundleUtilities)
        file(GLOB moduleDependencies LIST_DIRECTORIES false \"\${CMAKE_INSTALL_PREFIX}/wxFormBuilder.app/Contents/PlugIns/*.dylib\")
        fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/wxFormBuilder.app\" \"\${moduleDependencies}\" \"\")
        unset(moduleDependencies)
  
        find_program(install_name_tool_cmd NAMES install_name_tool REQUIRED)
        mark_as_advanced(install_name_tool_cmd)
        execute_process(COMMAND \"\${install_name_tool_cmd}\" -add_rpath @executable_path/../PlugIns \"\${CMAKE_INSTALL_PREFIX}/wxFormBuilder.app/Contents/MacOS/wxFormBuilder\")
  
        find_program(codesign_cmd NAMES codesign REQUIRED)
        mark_as_advanced(codesign_cmd)
        execute_process(COMMAND \"\${codesign_cmd}\" -s - -f --deep --verbose \"\${CMAKE_INSTALL_PREFIX}/wxFormBuilder.app\")
      "
    )
  endif()
endmacro()
