include(FindPackageHandleStandardArgs)

find_path(SFRAME_INCLUDE_DIR
  NAMES
    sframe.h
  PATH_SUFFIXES
    binutils
)
find_library(SFRAME_LIBRARY
  NAMES
    libsframe.a
  PATH_SUFFIXES
    binutils
)

find_package_handle_standard_args(SFRAME
  REQUIRED_VARS
    SFRAME_LIBRARY
    SFRAME_INCLUDE_DIR
)
mark_as_advanced(
  SFRAME_INCLUDE_DIR
  SFRAME_LIBRARY
)

if(SFRAME_FOUND AND NOT TARGET SFrame::SFrame)
  add_library(SFrame::SFrame STATIC IMPORTED)
  set_target_properties(SFrame::SFrame PROPERTIES
    IMPORTED_LOCATION "${SFRAME_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${SFRAME_INCLUDE_DIR}"
  )
endif()
