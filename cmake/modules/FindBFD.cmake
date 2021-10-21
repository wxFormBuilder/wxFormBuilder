include(FindPackageHandleStandardArgs)

find_path(BFD_INCLUDE_DIR
  NAMES
    bfd.h
  PATH_SUFFIXES
    binutils
)
find_library(BFD_LIBRARY
  NAMES
    libbfd.a
  PATH_SUFFIXES
    binutils
)

find_package_handle_standard_args(BFD
  REQUIRED_VARS
    BFD_LIBRARY
    BFD_INCLUDE_DIR
)
mark_as_advanced(
  BFD_INCLUDE_DIR
  BFD_LIBRARY
)

if(BFD_FOUND AND NOT TARGET BFD::BFD)
  add_library(BFD::BFD STATIC IMPORTED)
  set_target_properties(BFD::BFD PROPERTIES
    IMPORTED_LOCATION "${BFD_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${BFD_INCLUDE_DIR}"
  )
endif()
