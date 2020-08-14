#include(add_wxformbuilder_plugin_target)
include_directories(${CMAKE_SOURCE_DIR}/sdk/plugin_interface)

add_subdirectory(additional)
add_subdirectory(common)
add_subdirectory(containers)
add_subdirectory(forms)
add_subdirectory(layout)
