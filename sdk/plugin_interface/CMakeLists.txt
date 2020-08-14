set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_library(wxfb-plugin-interface STATIC
    xrcconv.cpp
    forms/wizard.cpp
)
target_link_libraries(wxfb-plugin-interface ${wxWidgets_LIBRARIES} ticpp)
target_include_directories(wxfb-plugin-interface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
