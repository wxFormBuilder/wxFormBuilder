cmake_minimum_required(VERSION 3.20...3.31)

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
    install(
      CODE
      "
        find_program(codesign_cmd NAMES codesign REQUIRED)
        mark_as_advanced(codesign_cmd)
        execute_process(COMMAND \"\${codesign_cmd}\" -s - -f --deep --verbose \"\${CMAKE_INSTALL_PREFIX}/wxFormBuilder.app\")
      "
    )
  endif()
endmacro()
