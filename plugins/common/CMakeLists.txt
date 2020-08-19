set(PLUGIN_NAME "common")

add_library(${PLUGIN_NAME} SHARED common.cpp)

target_link_libraries(${PLUGIN_NAME}
    ticpp
    wxfb-plugin-interface
)

install_plugin(${PLUGIN_NAME})
