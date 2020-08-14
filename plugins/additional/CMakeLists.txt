add_library(additional SHARED additional.cpp)

target_link_libraries(additional
    ticpp
    wxfb-plugin-interface
)

if(MSYS)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/libadditional${CMAKE_SHARED_LIBRARY_SUFFIX}
        DESTINATION ${INSTALL_LIB_DIR}/plugins/additional/
    )
elseif(UNIX)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/libadditional${CMAKE_SHARED_LIBRARY_SUFFIX}
        DESTINATION ${INSTALL_LIB_DIR}
    )
endif()
    
