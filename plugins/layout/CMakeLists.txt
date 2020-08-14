add_library(layout SHARED layout.cpp)

target_link_libraries(layout
    ticpp
    wxfb-plugin-interface
)

if(MSYS)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/liblayout${CMAKE_SHARED_LIBRARY_SUFFIX}
        DESTINATION ${INSTALL_LIB_DIR}/plugins/layout/
    )
elseif(UNIX)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/liblayout${CMAKE_SHARED_LIBRARY_SUFFIX}
        DESTINATION ${INSTALL_LIB_DIR}
    )
endif()
