include(FindPackageHandleStandardArgs)

find_path(Iberty_INCLUDE_DIR
  NAMES
    libiberty.h
  PATH_SUFFIXES
    libiberty
)
find_library(Iberty_LIBRARY
  NAMES
    libiberty.a
  PATH_SUFFIXES
    binutils
)

find_package_handle_standard_args(Iberty
  REQUIRED_VARS
    Iberty_LIBRARY
    Iberty_INCLUDE_DIR
)
mark_as_advanced(
  Iberty_INCLUDE_DIR
  Iberty_LIBRARY
)

if(Iberty_FOUND AND NOT TARGET Iberty::Iberty)
  add_library(Iberty::Iberty STATIC IMPORTED)
  set_target_properties(Iberty::Iberty PROPERTIES
    IMPORTED_LOCATION "${Iberty_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${Iberty_INCLUDE_DIR}"
  )
endif()
