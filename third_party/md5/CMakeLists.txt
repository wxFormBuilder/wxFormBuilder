add_library(md5_md5 OBJECT)
add_library(md5::md5 ALIAS md5_md5)

target_sources(md5_md5
  PRIVATE
    md5/md5.cc
    md5/md5.hh
)

target_include_directories(md5_md5
  INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)
