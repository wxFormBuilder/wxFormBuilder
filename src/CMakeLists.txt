if(NOT APPLE)
  file(RELATIVE_PATH libDir
    "${CMAKE_CURRENT_BINARY_DIR}/${WXFB_INSTALL_BINDIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/${WXFB_INSTALL_LIBDIR}"
  )
  set(pluginDir "${libDir}/wxformbuilder")
endif()

set(resourceDestination ${WXFB_INSTALL_DATADIR}/resources)
if(WXFB_STAGE_BUILD)
  set(stageResourceDestination "${WXFB_STAGE_DIR}/${resourceDestination}")
else()
  set(stageResourceDestination "")
endif()

add_executable(wxFormBuilder_app WIN32 MACOSX_BUNDLE)
add_executable(wxFormBuilder::app ALIAS wxFormBuilder_app)
set_target_properties(wxFormBuilder_app PROPERTIES
  OUTPUT_NAME "$<IF:$<OR:$<BOOL:${WIN32}>,$<BOOL:${APPLE}>>,wxFormBuilder,wxformbuilder>"
)

get_version(wxFormBuilder_app
  INPUT_FILE rad/version.cpp.in
  OUTPUT_FILE rad/version.cpp
)
get_revision(wxFormBuilder_app
  INPUT_FILE rad/revision.cpp.in
  OUTPUT_FILE rad/revision.cpp
  FOLDER "Application"
  REQUIRED
)

target_sources(wxFormBuilder_app
  PRIVATE
    maingui.cpp
    maingui.h
    $<$<BOOL:${WIN32}>:wxFormBuilder.rc>
    codegen/codegen.cpp
    codegen/codegen.h
    codegen/codeparser.cpp
    codegen/codeparser.h
    codegen/codewriter.cpp
    codegen/codewriter.h
    codegen/cppcg.cpp
    codegen/cppcg.h
    codegen/luacg.cpp
    codegen/luacg.h
    codegen/phpcg.cpp
    codegen/phpcg.h
    codegen/pythoncg.cpp
    codegen/pythoncg.h
    codegen/xrccg.cpp
    codegen/xrccg.h
    model/database.cpp
    model/database.h
    model/objectbase.cpp
    model/objectbase.h
    model/types.cpp
    model/types.h
    model/xrcfilter.cpp
    model/xrcfilter.h
    rad/about.cpp
    rad/about.h
    rad/appdata.cpp
    rad/appdata.h
    rad/auitabart.h
    rad/bitmaps.cpp
    rad/bitmaps.h
    rad/cmdproc.cpp
    rad/cmdproc.h
    rad/customkeys.cpp
    rad/customkeys.h
    rad/dialogfindcomponent.cpp
    rad/dialogfindcomponent.h
    rad/genericpanel.cpp
    rad/genericpanel.h
    rad/mainframe.cpp
    rad/mainframe.h
    rad/menueditor.cpp
    rad/menueditor.h
    rad/palette.cpp
    rad/palette.h
    rad/revision.h
    rad/title.cpp
    rad/title.h
    rad/version.h
    rad/wxfbevent.cpp
    rad/wxfbevent.h
    rad/wxfbmanager.cpp
    rad/wxfbmanager.h
    rad/codeeditor/codeeditor.cpp
    rad/codeeditor/codeeditor.h
    rad/cpppanel/cpppanel.cpp
    rad/cpppanel/cpppanel.h
    rad/dataobject/dataobject.cpp
    rad/dataobject/dataobject.h
    rad/designer/innerframe.cpp
    rad/designer/innerframe.h
    rad/designer/menubar.cpp
    rad/designer/menubar.h
    #rad/designer/resizablepanel.cpp
    #rad/designer/resizablepanel.h
    rad/designer/visualeditor.cpp
    rad/designer/visualeditor.h
    rad/designer/visualobj.cpp
    rad/designer/visualobj.h
    rad/designer/window_buttons.h
    rad/geninheritclass/geninhertclass.cpp
    rad/geninheritclass/geninhertclass.h
    rad/geninheritclass/geninhertclass_gui.cpp
    rad/geninheritclass/geninhertclass_gui.h
    rad/inspector/objinspect.cpp
    rad/inspector/objinspect.h
    rad/inspector/wxfbadvprops.cpp
    rad/inspector/wxfbadvprops.h
    rad/luapanel/luapanel.cpp
    rad/luapanel/luapanel.h
    rad/objecttree/objecttree.cpp
    rad/objecttree/objecttree.h
    rad/phppanel/phppanel.cpp
    rad/phppanel/phppanel.h
    rad/pythonpanel/pythonpanel.cpp
    rad/pythonpanel/pythonpanel.h
    rad/xrcpanel/xrcpanel.cpp
    rad/xrcpanel/xrcpanel.h
    rad/xrcpreview/xrcpreview.cpp
    rad/xrcpreview/xrcpreview.h
    utils/annoyingdialog.cpp
    utils/annoyingdialog.h
    utils/debug.h
    utils/filetocarray.cpp
    utils/filetocarray.h
    utils/m_wxfb.cpp
    utils/stringutils.cpp
    utils/stringutils.h
    utils/typeconv.cpp
    utils/typeconv.h
    utils/wxfbdefs.h
    utils/wxfbexception.h
    utils/wxfbipc.cpp
    utils/wxfbipc.h
    utils/wxlogstring.h
)
set(commonDefinitions
  default
  icons
  objtypes
)
set(templateDefinitions
  default
  properties
)
set(commonResources
  about.html
)
set(commonIcons
  balign.png
  bottom.xpm
  c++.png
  chalign.png
  copy.xpm
  cpp.png
  cut.png
  cvalign.png
  delete.png
  designer.png
  events.png
  expand.png
  generate.png
  h.png
  lalign.png
  left.xpm
  logo.png
  logo.xpm
  lua.png
  new.png
  open.png
  paste.png
  php.png
  prefs.xpm
  project.xpm
  properties.png
  python.png
  ralign.png
  redo.png
  right.xpm
  save.png
  stretch.png
  swap.png
  talign.png
  top.xpm
  undo.png
  wxwin16x16.xpm
  xrc.png
)
if(APPLE)
  set(appleResources
    "${PROJECT_SOURCE_DIR}/data/platform/macos/docicon.icns"
    "${PROJECT_SOURCE_DIR}/data/platform/macos/icon.icns"
  )
  target_sources(wxFormBuilder_app PRIVATE ${appleResources})
  set_target_properties(wxFormBuilder_app PROPERTIES RESOURCE "${appleResources}")
endif()

target_compile_definitions(wxFormBuilder_app
  PRIVATE
    BOOST_DLL_USE_STD_FS
    $<$<CONFIG:DEBUG>:__WXFB_DEBUG__>
)
target_include_directories(wxFormBuilder_app
  PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<BUILD_INTERFACE:$<$<BOOL:${WIN32}>:${PROJECT_SOURCE_DIR}/data/platform/windows>>"
)

target_link_libraries(wxFormBuilder_app
  PRIVATE
    wxFormBuilder::plugin-interface
    ticpp::ticpp
    Boost::headers
    md5::md5
    stack_trace::stack
)
if(TARGET wxWidgets::wxWidgets)
  target_link_libraries(wxFormBuilder_app PUBLIC wxWidgets::wxWidgets)
else()
  target_link_libraries(wxFormBuilder_app PUBLIC ${wxWidgets_LIBRARIES})
endif()

set_target_properties(wxFormBuilder_app PROPERTIES
  MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
  MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
  MACOSX_BUNDLE_GUI_IDENTIFIER "org.wxformbuilder.wxFormBuilder"
  MACOSX_BUNDLE_ICON_FILE "icon.icns"
  MACOSX_BUNDLE_INFO_PLIST "${PROJECT_SOURCE_DIR}/data/platform/macos/Info.plist.in"
)
if(APPLE)
  set_target_properties(wxFormBuilder_app PROPERTIES INSTALL_RPATH "@executable_path/../Frameworks;@executable_path/../PlugIns")
else()
  set_target_properties(wxFormBuilder_app PROPERTIES INSTALL_RPATH "$ORIGIN/${libDir};$ORIGIN/${pluginDir}")
endif()

set_target_properties(wxFormBuilder_app PROPERTIES FOLDER "Application")
wxfb_target_source_groups(wxFormBuilder_app)

# Use the same dependencies set like the plugins
install(TARGETS wxFormBuilder_app
  RUNTIME_DEPENDENCY_SET wxFormBuilder_dependencies
  BUNDLE
    DESTINATION ${WXFB_INSTALL_BINDIR}
  RUNTIME
    DESTINATION ${WXFB_INSTALL_BINDIR}
)

wxfb_target_definitions(wxFormBuilder_app
  INPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/resources"
  OUTPUT_DIRECTORY "${stageResourceDestination}"
  INSTALL_DIRECTORY "${resourceDestination}"
  FOLDER "Application"
  COMMON ${commonDefinitions}
  TEMPLATES ${templateDefinitions}
)

wxfb_target_resources(wxFormBuilder_app
  INPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/resources"
  OUTPUT_DIRECTORY "${stageResourceDestination}"
  INSTALL_DIRECTORY "${resourceDestination}"
  FOLDER "Application"
  RESOURCES ${commonResources}
  ICONS ${commonIcons}
)

install(
  FILES
    "${PROJECT_SOURCE_DIR}/Changelog.txt"
    "${PROJECT_SOURCE_DIR}/LICENSE"
  DESTINATION ${WXFB_INSTALL_DATADIR}
)

if(NOT WIN32 AND NOT APPLE)
  install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/data/platform/linux/share/"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
  )
  install(
    CODE
    "
      find_program(update-mime-database_cmd NAMES update-mime-database REQUIRED)
      mark_as_advanced(update-mime-database_cmd)
      execute_process(COMMAND \"\${update-mime-database_cmd}\" \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/mime\")
    "
  )
endif()

if(WIN32)
  # The installation of runtime dependencies does not take the set paths into account and
  # only searches in system default paths. Therefor paths of libraries not in such locations
  # need to be specified manually. Depending on the used compiler these are different.
  set(extraDirectories "")
  if(MSVC)
    # The compiler runtime libraries are installed in system default paths, because it is expected
    # that the runtime distributables are getting installed along the application, these need to get
    # filtered. The wxWidgets library uses the win32 search style and therefor the library path is defined
    # (TODO: Unless the CMake build gets imported, what to do then?)
    list(APPEND extraDirectories "${wxWidgets_LIB_DIR}")
    install(RUNTIME_DEPENDENCY_SET wxFormBuilder_dependencies
      DIRECTORIES
        ${extraDirectories}
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
      execute_process(COMMAND "${cygpath_cmd}" -m /mingw${targetBitness}/bin OUTPUT_VARIABLE msysSystemDir OUTPUT_STRIP_TRAILING_WHITESPACE)
      list(APPEND extraDirectories "${msysSystemDir}")
      unset(msysSystemDir)
    else()
      # In plain MinGW environments it is more difficult to determine the paths in a system independent manner.
      # As best effort search the path of the compiler and hope that contains all system libraries.
      find_path(compilerSystemDir NAMES gcc.exe REQUIRED)
      mark_as_advanced(compilerSystemDir)
      list(APPEND extraDirectories "${compilerSystemDir}")
      # Directories of additional libraries need to be added as well
      list(APPEND extraDirectories "${wxWidgets_LIB_DIR}")
    endif()
    install(RUNTIME_DEPENDENCY_SET wxFormBuilder_dependencies
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
    unset(targetBitness)
  endif()
  unset(extraDirectories)
endif()
