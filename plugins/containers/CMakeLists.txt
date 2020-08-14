add_library(containers SHARED containers.cpp)

target_link_libraries(containers
    ticpp
    wxfb-plugin-interface
)

if(MSYS)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/libcontainers${CMAKE_SHARED_LIBRARY_SUFFIX}
        DESTINATION ${INSTALL_LIB_DIR}/plugins/containers/
    )
elseif(UNIX)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/libcontainers${CMAKE_SHARED_LIBRARY_SUFFIX}
        DESTINATION ${INSTALL_LIB_DIR}
    )
endif()
