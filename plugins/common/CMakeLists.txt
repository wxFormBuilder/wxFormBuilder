add_library(common SHARED common.cpp)

target_link_libraries(common
    ticpp
    wxfb-plugin-interface
)

if(MSYS)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/libcommon${CMAKE_SHARED_LIBRARY_SUFFIX}
        DESTINATION ${INSTALL_LIB_DIR}/plugins/common/
    )
elseif(UNIX)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/libcommon${CMAKE_SHARED_LIBRARY_SUFFIX}
        DESTINATION ${INSTALL_LIB_DIR}
    )
endif()
