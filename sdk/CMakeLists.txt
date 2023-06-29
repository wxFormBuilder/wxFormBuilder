add_library(wxFormBuilder_sdk_common STATIC)
add_library(wxFormBuilder::sdk_common ALIAS wxFormBuilder_sdk_common)
set_target_properties(wxFormBuilder_sdk_common PROPERTIES
  OUTPUT_NAME "common"
)

target_sources(wxFormBuilder_sdk_common
  PRIVATE
    common/xmlutils.cpp
    common/xmlutils.h
)

target_include_directories(wxFormBuilder_sdk_common
  INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)

target_link_libraries(wxFormBuilder_sdk_common
  PUBLIC
    wxWidgets::wxWidgets
    tinyxml2::tinyxml2
)

set_target_properties(wxFormBuilder_sdk_common PROPERTIES FOLDER "Sdk")
wxfb_target_source_groups(wxFormBuilder_sdk_common)


add_library(wxFormBuilder_sdk_plugin-interface STATIC)
add_library(wxFormBuilder::sdk_plugin-interface ALIAS wxFormBuilder_sdk_plugin-interface)
set_target_properties(wxFormBuilder_sdk_plugin-interface PROPERTIES
  OUTPUT_NAME "plugin-interface"
)

target_sources(wxFormBuilder_sdk_plugin-interface
  PRIVATE
    plugin_interface/component.h
    plugin_interface/fontcontainer.h
    plugin_interface/plugin.cpp
    plugin_interface/plugin.h
    plugin_interface/xrcconv.cpp
    plugin_interface/xrcconv.h
    plugin_interface/forms/wizard.cpp
    plugin_interface/forms/wizard.h
    plugin_interface/forms/wizardbase.cpp
    plugin_interface/forms/wizardbase.h
)

target_compile_definitions(wxFormBuilder_sdk_plugin-interface
  INTERFACE
    "$<$<IN_LIST:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY;MODULE_LIBRARY>:BUILD_DLL>"
)
target_include_directories(wxFormBuilder_sdk_plugin-interface
  INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)

target_link_libraries(wxFormBuilder_sdk_plugin-interface
  PUBLIC
    wxFormBuilder::sdk_common
    wxWidgets::wxWidgets
    tinyxml2::tinyxml2
)

set_target_properties(wxFormBuilder_sdk_plugin-interface PROPERTIES FOLDER "Sdk")
wxfb_target_source_groups(wxFormBuilder_sdk_plugin-interface)
