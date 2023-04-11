add_library(wxFormBuilder_plugin-interface STATIC)
add_library(wxFormBuilder::plugin-interface ALIAS wxFormBuilder_plugin-interface)
set_target_properties(wxFormBuilder_plugin-interface PROPERTIES
  OUTPUT_NAME "plugin-interface"
)

target_sources(wxFormBuilder_plugin-interface
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

target_compile_definitions(wxFormBuilder_plugin-interface
  INTERFACE
    "$<$<IN_LIST:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY;MODULE_LIBRARY>:BUILD_DLL>"
)
target_include_directories(wxFormBuilder_plugin-interface
  INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)

target_link_libraries(wxFormBuilder_plugin-interface
  PUBLIC
    wxWidgets::wxWidgets
    ticpp::ticpp
)

set_target_properties(wxFormBuilder_plugin-interface PROPERTIES FOLDER "Sdk")
wxfb_target_source_groups(wxFormBuilder_plugin-interface)
