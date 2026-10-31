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
    # Libraries that are referenced via @rpath by the application, the plugins or their dependencies can only be found
    # if their location is known. The system search paths cannot be used, because the libraries are not necessarily
    # installed in a standard location.
    set(searchDirectories "")
    if(DEFINED wxWidgets_LIBRARY_DIRS)
      list(APPEND searchDirectories ${wxWidgets_LIBRARY_DIRS})
    endif()
    if(DEFINED ENV{HOMEBREW_PREFIX})
      list(APPEND searchDirectories "$ENV{HOMEBREW_PREFIX}/lib")
    endif()
    foreach(prefix IN LISTS CMAKE_PREFIX_PATH CMAKE_SYSTEM_PREFIX_PATH)
      list(APPEND searchDirectories "${prefix}/lib")
    endforeach()
    list(REMOVE_DUPLICATES searchDirectories)
    # Copy the required libraries into the bundle and make the whole bundle self-contained.
    # This must run after all other installation steps, because it operates on the installed bundle.
    install(
      CODE
      "
        include(\"${PROJECT_SOURCE_DIR}/cmake/macos-bundle.cmake\")
        wxfb_macos_bundle_fixup(
          \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/wxFormBuilder.app\"
          EXECUTABLE wxFormBuilder
          DIRECTORIES \"${searchDirectories}\"
        )
      "
    )
    unset(searchDirectories)
    unset(prefix)
  endif()
endmacro()
