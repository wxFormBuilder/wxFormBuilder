set(PLUGIN_NAME "forms")

add_library(${PLUGIN_NAME} SHARED forms.cpp)

target_link_libraries(${PLUGIN_NAME}
    ticpp
    wxfb-plugin-interface
)

install_plugin(${PLUGIN_NAME})
