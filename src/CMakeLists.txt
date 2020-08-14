set(wxFB_SRCS
    maingui.cpp
    md5/md5.cc
    dbg_stack_trace/stack.cpp
    rad/about.cpp
    rad/appdata.cpp
    rad/bitmaps.cpp
    rad/cmdproc.cpp
    rad/customkeys.cpp
    rad/genericpanel.cpp
    rad/mainframe.cpp
    rad/menueditor.cpp
    rad/palette.cpp
    rad/title.cpp
    rad/wxfbevent.cpp
    rad/wxfbmanager.cpp
    rad/codeeditor/codeeditor.cpp
    rad/cpppanel/cpppanel.cpp
    rad/dataobject/dataobject.cpp
    rad/designer/innerframe.cpp
    rad/designer/menubar.cpp
    rad/designer/resizablepanel.cpp
    rad/designer/visualeditor.cpp
    rad/designer/visualobj.cpp
    rad/geninheritclass/geninhertclass.cpp
    rad/geninheritclass/geninhertclass_gui.cpp
    rad/inspector/objinspect.cpp
    rad/inspector/wxfbadvprops.cpp
    rad/luapanel/luapanel.cpp
    rad/objecttree/objecttree.cpp
    rad/phppanel/phppanel.cpp
    rad/pythonpanel/pythonpanel.cpp
    rad/xrcpanel/xrcpanel.cpp
    rad/xrcpreview/xrcpreview.cpp
    utils/filetocarray.cpp
    utils/annoyingdialog.cpp
    utils/stringutils.cpp
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

if(MSYS)
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

if(MSYS)
    find_path(BINUTILS_BASE_INCLUDE_DIR binutils/bfd.h)
    target_include_directories(wxFormBuilder
        PRIVATE ${BINUTILS_BASE_INCLUDE_DIR}/binutils
    )
endif()

if(MSYS)
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
        imagehlp
    )

    find_path(MSYS_PATH include/binutils/bfd.h)
    set(MINGW_BIN_DIR "${MSYS_PATH}/bin")
    set(MINGW_MODULES
        libexpat
        libgcc
        libiconv
        libintl
        libjpeg
        liblzma
        libpng
        libstdc++
        libtiff
        libwinpthread
        libzstd
        zlib
        wx
    )
    
    foreach(mod IN LISTS MINGW_MODULES)
        file(GLOB mods "${MINGW_BIN_DIR}/${mod}*.dll")
        list(APPEND MINGW_FILES ${mods})
    endforeach()
    
    install(FILES ${MINGW_FILES} DESTINATION ${INSTALL_BIN_DIR})

endif()

install(TARGETS wxFormBuilder
    BUNDLE DESTINATION ${INSTALL_BIN_DIR}
    RUNTIME DESTINATION ${INSTALL_BIN_DIR}
)


install(DIRECTORY
    ${CMAKE_SOURCE_DIR}/output/xml
    ${CMAKE_SOURCE_DIR}/output/resources
    ${CMAKE_SOURCE_DIR}/output/plugins
    DESTINATION ${INSTALL_SHARE_DIR}
)

