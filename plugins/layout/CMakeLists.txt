set(PLUGIN_NAME "layout")

add_library(${PLUGIN_NAME} SHARED layout.cpp)

target_link_libraries(${PLUGIN_NAME}
    ticpp
    wxfb-plugin-interface
)

install_plugin(${PLUGIN_NAME})
