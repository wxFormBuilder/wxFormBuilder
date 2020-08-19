set(PLUGIN_NAME "containers")

add_library(${PLUGIN_NAME} SHARED containers.cpp)

target_link_libraries(${PLUGIN_NAME}
    ticpp
    wxfb-plugin-interface
)

install_plugin(${PLUGIN_NAME})
