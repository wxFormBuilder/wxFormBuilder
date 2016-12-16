set(wxFB_SRCS
    maingui.cpp
    md5/md5.cc
    dbg_stack_trace/stack.cpp
    rad/wxfbmanager.cpp
    rad/bitmaps.cpp
    rad/customkeys.cpp
    rad/xrcpanel/xrcpanel.cpp
    rad/pythonpanel/pythonpanel.cpp
    rad/palette.cpp
    rad/xrcpreview/xrcpreview.cpp
    rad/codeeditor/codeeditor.cpp
    rad/dataobject/dataobject.cpp
    rad/appdata.cpp
    rad/cpppanel/cpppanel.cpp
    rad/title.cpp
    rad/menueditor.cpp
    rad/about.cpp
    rad/mainframe.cpp
    rad/objecttree/objecttree.cpp
    rad/geninheritclass/geninhertclass.cpp
    rad/geninheritclass/geninhertclass_gui.cpp
    rad/genericpanel.cpp
    rad/phppanel/phppanel.cpp
    rad/wxfbevent.cpp
    rad/cmdproc.cpp
    rad/inspector/wxfbadvprops.cpp
    rad/inspector/objinspect.cpp
    rad/designer/resizablepanel.cpp
    rad/designer/visualeditor.cpp
    rad/designer/innerframe.cpp
    rad/designer/visualobj.cpp
    rad/designer/menubar.cpp
    rad/luapanel/luapanel.cpp
    utils/filetocarray.cpp
    utils/annoyingdialog.cpp
    utils/stringutils.cpp
    utils/encodingutils.cpp
    utils/wxfbipc.cpp
    utils/typeconv.cpp
    codegen/phpcg.cpp
    codegen/codeparser.cpp
    codegen/xrccg.cpp
    codegen/luacg.cpp
    codegen/cppcg.cpp
    codegen/codegen.cpp
    codegen/pythoncg.cpp
    codegen/codewriter.cpp
    model/xrcfilter.cpp
    model/objectbase.cpp
    model/types.cpp
    model/database.cpp
)

if(WIN32)
    list(APPEND wxFB_SRCS wxFormBuilder.rc)
endif()

add_executable(wxFormBuilder WIN32
    ${wxFB_SRCS}
)

target_include_directories(wxFormBuilder
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(wxFormBuilder
    ${wxWidgets_LIBRARIES}
    ticpp
    wxfb-plugin-interface
)

if(MINGW)
    find_path(BINUTILS_BASE_INCLUDE_DIR binutils/bfd.h)
    target_include_directories(wxFormBuilder
        PRIVATE ${BINUTILS_BASE_INCLUDE_DIR}/binutils
    )
endif()

if(WIN32)
    if(MSVC)
        target_link_libraries(wxFormBuilder Dbghelp)
    else()
        find_library(BFD_LIB NAMES binutils/libbfd.a libbfd.a)
        find_library(IBERTY_LIB NAMES binutils/libiberty.a libiberty.a)
        find_package(ZLIB REQUIRED)
        find_package(Intl REQUIRED)

        target_link_libraries(wxFormBuilder
            ${BFD_LIB}
            ${IBERTY_LIB}
            ${Intl_LIBRARIES}
            ${ZLIB_LIBRARIES}
            psapi
            imagehlp)
    endif()
endif()

install(TARGETS wxFormBuilder
    RUNTIME DESTINATION bin
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../output/xml 
    ${CMAKE_CURRENT_SOURCE_DIR}/../output/resources
    DESTINATION share/wxformbuilder)
