if(WIN32)
  if(MINGW)
    find_package(BFD REQUIRED)
    find_package(Iberty REQUIRED)
    # Non-MSYS2-MinGW builds seem not to require Intl but only ZLIB
    if(MSYS)
      find_package(Intl REQUIRED)
    else()
      find_package(Intl QUIET)
    endif()
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

configure_file(rad/version.cpp.in rad/version.cpp @ONLY)
add_custom_target(wxFormBuilder_revision
  COMMAND "${CMAKE_COMMAND}"
    -D "gitDirectory=${PROJECT_SOURCE_DIR}"
    -D "gitTemplate=${CMAKE_CURRENT_SOURCE_DIR}/rad/revision.cpp.in"
    -D "gitOutput=${CMAKE_CURRENT_BINARY_DIR}/rad/revision.cpp"
    -P "${PROJECT_SOURCE_DIR}/cmake/gitrevision.cmake"
  VERBATIM
)

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
    rad/revision.h
    rad/revision.cpp.in
    "${CMAKE_CURRENT_BINARY_DIR}/rad/revision.cpp"
    rad/title.h
    rad/title.cpp
    rad/version.h
    rad/version.cpp.in
    "${CMAKE_CURRENT_BINARY_DIR}/rad/version.cpp"
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
    BOOST_DLL_USE_STD_FS
    WXFB_PLUGINS_RESOLVE
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
    Boost::headers
)
if(WIN32)
  if(MINGW)
    target_link_libraries(wxFormBuilder_app
      PRIVATE
        BFD::BFD
        Iberty::Iberty
        $<TARGET_NAME_IF_EXISTS:Intl::Intl>
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
add_dependencies(wxFormBuilder_app wxFormBuilder_revision)

set_target_properties(wxFormBuilder_app PROPERTIES
  MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
  MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
  MACOSX_BUNDLE_GUI_IDENTIFIER "org.wxformbuilder.wxFormBuilder"
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

# Use the same dependencies set like the plugins
install(TARGETS wxFormBuilder_app
  RUNTIME_DEPENDENCY_SET wxFormBuilder_deps
  BUNDLE
    DESTINATION .
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)
# TODO: After changing the layout, maybe use an own CMakeLists for resources and even own targets for them
install(
  FILES
    ../output/Changelog.txt
    ../LICENSE
  DESTINATION ${CMAKE_INSTALL_DATADIR}
)
install(DIRECTORY ../output/resources
  DESTINATION ${CMAKE_INSTALL_DATADIR}
)
install(DIRECTORY ../output/xml
  DESTINATION ${CMAKE_INSTALL_DATADIR}
)

if(NOT WIN32 AND NOT APPLE)
  install(
    FILES ../install/linux/data/gnome/usr/share/applications/wxformbuilder.desktop
    RENAME org.wxformbuilder.wxFormBuilder.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
  )
  install(
    FILES ../install/linux/data/gnome/usr/share/appdata/org.wxformbuilder.wxformbuilder.appdata.xml
    RENAME org.wxformbuilder.wxFormBuilder.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo
  )
  install(
    FILES ../install/linux/data/gnome/usr/share/pixmaps/wxformbuilder.png
    RENAME org.wxformbuilder.wxFormBuilder.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps
  )
  install(
    FILES ../install/linux/data/gnome/usr/share/mime/packages/org.wxformbuilder.wxFormBuilder.xml
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/mime/packages
  )
  install(
    FILES ../install/linux/data/gnome/usr/share/icons/gnome/48x48/mimetypes/gnome-mime-application-x-wxformbuilder.png
    RENAME org.wxformbuilder.wxFormBuilder.application-x-wxformbuilder.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/mimetypes
  )
  install(
    CODE
    "
      find_program(update-mime-database_cmd NAMES update-mime-database REQUIRED)
      mark_as_advanced(update-mime-database_cmd)
      execute_process(COMMAND \"\${update-mime-database_cmd}\" \"${CMAKE_INSTALL_FULL_DATAROOTDIR}/mime\")
    "
  )
endif()

if(WIN32)
  # The installation of runtime dependencies does not take the set paths into account and
  # only searches in system default paths. Therefor paths of libraries not in such locations
  # need to be specified manually. Depending on the used compiler these are different.
  if(MSVC)
    # The compiler runtime libraries are installed in system default paths, because it is expected
    # that the runtime distributables are getting installed along the application, these need to get
    # filtered. The wxWidgets library uses the win32 search style and therefor the library path is defined
    # (TODO: Unless the CMake build gets imported, what to do then?)
    install(RUNTIME_DEPENDENCY_SET wxFormBuilder_deps
      DIRECTORIES
        "${wxWidgets_LIB_DIR}"
      PRE_EXCLUDE_REGEXES
        [[api-ms-win-.*]]
        [[ext-ms-.*]]
        [[kernel32\.dll]]
        [[msvcp.*\.dll]]
        [[vcruntime.*\.dll]]
      POST_EXCLUDE_REGEXES
        # For some reason some dependencies use forward slashes, some backward slashes
        [[.*[/\]system32/.*\.dll]]
    )
  elseif(MINGW)
    # This environment has its own search paths which are not taken into account and must be specified.
    # Depending on the bitness of the target architecture different directories are used.
    set(extraDirectories)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(targetBitness 64)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
      set(targetBitness 32)
    else()
      message(FATAL_ERROR "Unknown target bitness: ${CMAKE_SIZEOF_VOID_P} bytes")
    endif()
    if(MSYS)
      # The "virtual" paths used by the MSYS environment cannot be used, they must be translated into physical
      # paths of the system.
      find_program(cygpath_cmd NAMES cygpath REQUIRED)
      mark_as_advanced(cygpath_cmd)
      # The mixed mode output equals the CMake path format
      execute_process(COMMAND "${cygpath_cmd}" -m "/mingw${targetBitness}/bin" OUTPUT_VARIABLE msysSystemDir OUTPUT_STRIP_TRAILING_WHITESPACE)
      list(APPEND extraDirectories "${msysSystemDir}")
    else()
      # In plain MinGW environments it is more difficult to determine the paths in a system independent manner.
      # As best effort search the path of the compiler and hope that contains all system libraries.
      find_path(compilerSystemDir NAMES gcc.exe REQUIRED)
      mark_as_advanced(compilerSystemDir)
      list(APPEND extraDirectories "${compilerSystemDir}")
      # Directories of additional libraries need to be added as well
      list(APPEND extraDirectories "${wxWidgets_LIB_DIR}")
    endif()
    install(RUNTIME_DEPENDENCY_SET wxFormBuilder_deps
      DIRECTORIES
        ${extraDirectories}
      PRE_EXCLUDE_REGEXES
        [[api-ms-win-.*]]
        [[ext-ms-.*]]
        [[kernel32\.dll]]
        [[user32\.dll]]
        [[msvcrt.*\.dll]]
      POST_EXCLUDE_REGEXES
        # For some reason some dependencies use forward slashes, some backward slashes
        [[.*[/\]system32/.*\.dll]]
    )
  endif()
endif()
