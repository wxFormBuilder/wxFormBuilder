if(WIN32)
  if(MINGW)
    find_package(BFD REQUIRED)
    find_package(Iberty REQUIRED)
    # TODO: Non-MSYS2-MinGW seems to not require Intl but only ZLIB
    find_package(Intl REQUIRED)
    find_package(ZLIB REQUIRED)
  endif()
endif()

if(NOT APPLE)
  file(RELATIVE_PATH libDir
    "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
  )
  set(pluginDir ${libDir}/wxformbuilder)
endif()

add_executable(wxFormBuilder_app WIN32 MACOSX_BUNDLE)
add_executable(wxFormBuilder::app ALIAS wxFormBuilder_app)
set_target_properties(wxFormBuilder_app PROPERTIES
  OUTPUT_NAME $<IF:$<OR:$<BOOL:${WIN32}>,$<BOOL:${APPLE}>>,wxFormBuilder,wxformbuilder>
)

target_sources(wxFormBuilder_app
  PRIVATE
    maingui.h
    maingui.cpp
    $<$<BOOL:${WIN32}>:wxFormBuilder.rc>
    codegen/codegen.h
    codegen/codegen.cpp
    codegen/codeparser.h
    codegen/codeparser.cpp
    codegen/codewriter.h
    codegen/codewriter.cpp
    codegen/cppcg.h
    codegen/cppcg.cpp
    codegen/luacg.h
    codegen/luacg.cpp
    codegen/phpcg.h
    codegen/phpcg.cpp
    codegen/pythoncg.h
    codegen/pythoncg.cpp
    codegen/xrccg.h
    codegen/xrccg.cpp
    dbg_stack_trace/stack.hpp
    dbg_stack_trace/stack.cpp
    md5/md5.hh
    md5/md5.cc
    model/database.h
    model/database.cpp
    model/objectbase.h
    model/objectbase.cpp
    model/types.h
    model/types.cpp
    model/xrcfilter.h
    model/xrcfilter.cpp
    rad/about.h
    rad/about.cpp
    rad/appdata.h
    rad/appdata.cpp
    rad/auitabart.h
    rad/bitmaps.h
    rad/bitmaps.cpp
    rad/cmdproc.h
    rad/cmdproc.cpp
    rad/customkeys.h
    rad/customkeys.cpp
    rad/genericpanel.h
    rad/genericpanel.cpp
    rad/mainframe.h
    rad/mainframe.cpp
    rad/menueditor.h
    rad/menueditor.cpp
    rad/palette.h
    rad/palette.cpp
    rad/title.h
    rad/title.cpp
    rad/wxfbevent.h
    rad/wxfbevent.cpp
    rad/wxfbmanager.h
    rad/wxfbmanager.cpp
    rad/codeeditor/codeeditor.h
    rad/codeeditor/codeeditor.cpp
    rad/cpppanel/cpppanel.h
    rad/cpppanel/cpppanel.cpp
    rad/dataobject/dataobject.h
    rad/dataobject/dataobject.cpp
    rad/designer/innerframe.h
    rad/designer/innerframe.cpp
    rad/designer/menubar.h
    rad/designer/menubar.cpp
    rad/designer/visualeditor.h
    rad/designer/visualeditor.cpp
    rad/designer/visualobj.h
    rad/designer/visualobj.cpp
    rad/designer/window_buttons.h
    rad/geninheritclass/GenInheritedDlg.fbp
    rad/geninheritclass/geninhertclass_gui.h
    rad/geninheritclass/geninhertclass_gui.cpp
    rad/geninheritclass/geninhertclass.h
    rad/geninheritclass/geninhertclass.cpp
    rad/inspector/objinspect.h
    rad/inspector/objinspect.cpp
    rad/inspector/wxfbadvprops.h
    rad/inspector/wxfbadvprops.cpp
    rad/luapanel/luapanel.h
    rad/luapanel/luapanel.cpp
    rad/objecttree/objecttree.h
    rad/objecttree/objecttree.cpp
    rad/phppanel/phppanel.h
    rad/phppanel/phppanel.cpp
    rad/pythonpanel/pythonpanel.h
    rad/pythonpanel/pythonpanel.cpp
    rad/xrcpanel/xrcpanel.h
    rad/xrcpanel/xrcpanel.cpp
    rad/xrcpreview/xrcpreview.h
    rad/xrcpreview/xrcpreview.cpp
    utils/annoyingdialog.h
    utils/annoyingdialog.cpp
    utils/debug.h
    utils/filetocarray.h
    utils/filetocarray.cpp
    utils/m_wxfb.cpp
    utils/stringutils.h
    utils/stringutils.cpp
    utils/typeconv.h
    utils/typeconv.cpp
    utils/wxfbdefs.h
    utils/wxfbexception.h
    utils/wxfbipc.h
    utils/wxfbipc.cpp
    utils/wxlogstring.h
)
if(APPLE)
  set(appleResources
    ../install/macosx/icon.icns
    ../install/macosx/docicon.icns
  )
  target_sources(wxFormBuilder_app
    PRIVATE
      ${appleResources}
  )
  set_target_properties(wxFormBuilder_app PROPERTIES
    RESOURCE "${appleResources}"
  )
endif()

target_compile_definitions(wxFormBuilder_app
  PRIVATE
    $<$<CONFIG:DEBUG>:__WXFB_DEBUG__>
)
target_include_directories(wxFormBuilder_app
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(wxFormBuilder_app
  PRIVATE
    wxFormBuilder::plugin-interface
    ticpp::ticpp
    ${wxWidgets_LIBRARIES}
)
if(WIN32)
  if(MINGW)
    target_link_libraries(wxFormBuilder_app
      PRIVATE
        BFD::BFD
        Iberty::Iberty
        Intl::Intl
        ZLIB::ZLIB
        psapi # TODO: Only required for systems older than Windows 7
    )
  endif()
  target_link_libraries(wxFormBuilder_app
    PRIVATE
      imagehlp
  )
else()
  target_link_libraries(wxFormBuilder_app
    PRIVATE
      dl
  )
endif()

set_target_properties(wxFormBuilder_app PROPERTIES
  MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
  MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
  MACOSX_BUNDLE_GUI_IDENTIFIER "org.wxformbuilder.wxformbuilder"
  MACOSX_BUNDLE_ICON_FILE "icon.icns"
  MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/../install/macosx/Info.plist.in"
)
if(APPLE)
  set_target_properties(wxFormBuilder_app PROPERTIES
    INSTALL_RPATH "@executable_path/../Frameworks;@executable_path/../PlugIns"
  )
else()
  set_target_properties(wxFormBuilder_app PROPERTIES
    INSTALL_RPATH "$ORIGIN/${libDir};$ORIGIN/${pluginDir}"
  )
endif()

install(TARGETS wxFormBuilder_app
  BUNDLE
    DESTINATION .
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)
# TODO: After changing the layout, maybe use an own CMakeLists for resources and even own targets for them
install(
  FILES
    ../output/Changelog.txt
    ../output/license.txt
  DESTINATION ${CMAKE_INSTALL_DATADIR}
)
install(DIRECTORY ../output/resources
  DESTINATION ${CMAKE_INSTALL_DATADIR}
)
install(DIRECTORY ../output/xml
  DESTINATION ${CMAKE_INSTALL_DATADIR}
)
