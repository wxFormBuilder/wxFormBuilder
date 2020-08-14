add_library(forms SHARED forms.cpp)

target_link_libraries(forms
    ticpp
    wxfb-plugin-interface
)

if(MSYS)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/libforms${CMAKE_SHARED_LIBRARY_SUFFIX}
        DESTINATION ${INSTALL_LIB_DIR}/plugins/forms/
    )
elseif(UNIX)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/libforms${CMAKE_SHARED_LIBRARY_SUFFIX}
        DESTINATION ${INSTALL_LIB_DIR}
    )
endif()
