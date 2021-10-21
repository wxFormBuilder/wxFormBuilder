add_library(wxFormBuilder_plugin-interface STATIC)
add_library(wxFormBuilder::plugin-interface ALIAS wxFormBuilder_plugin-interface)
set_target_properties(wxFormBuilder_plugin-interface PROPERTIES
  OUTPUT_NAME plugin-interface
)

target_sources(wxFormBuilder_plugin-interface
  PRIVATE
    component.h
    fontcontainer.h
    plugin.h
    xrcconv.h
    xrcconv.cpp
    forms/wizard.h
    forms/wizard.cpp
    forms/wizard.fbp
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
    ticpp::ticpp
    ${wxWidgets_LIBRARIES}
)
