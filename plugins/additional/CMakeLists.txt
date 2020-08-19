set(PLUGIN_NAME "additional")

add_library(${PLUGIN_NAME} SHARED additional.cpp)

target_link_libraries(${PLUGIN_NAME}
    ticpp
    wxfb-plugin-interface
)

install_plugin(${PLUGIN_NAME})
