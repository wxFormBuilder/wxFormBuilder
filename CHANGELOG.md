# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

### Added

- Add 'Load From SVG Resource' for bitmap property ([#817](https://github.com/wxFormBuilder/wxFormBuilder/pull/817))
- Add project option to use native eol-style for generated files
- Add a separate property to propGridItem to specify the name ([#815](https://github.com/wxFormBuilder/wxFormBuilder/issues/815))
- Add subclassing support for classes where missing for Python

### Changed

- BREAKING CHANGE: Data type `text` is now single line only instead of multiline. Many properties use this data type although they
  should contain single line values only. If such properties do contain multiline values, silent data loss will happen when these
  values are modified.
- Some properties now use a numeric data type instead of a text data type. These properties must only contain numeric values,
  this change should not lead to silent data loss because text values would have generated broken code.
- Reorder project properties for clarity
- Move some project C++ properties into global scope because they are in fact used by all languages
- Use a common prefix for language specific project properties to prevent name clashes
- Use more sensible default values for project properties

### Fixed

- Fix wxMediaControl Player Controls generated Python code ([#809](https://github.com/wxFormBuilder/wxFormBuilder/issues/809))
- Fix PHP code generation for objects with base classes
- Fix name clash of project property `disconnect_mode` between Python and PHP
- Prevent possible crash when loading a project file
- Code for property grid manager code generated with an error ([#805](https://github.com/wxFormBuilder/wxFormBuilder/issues/805))

## [4.1.0] - 2024-02-26

### Changed

- Update required wxWidgets version to 3.2

### Fixed

- Prevent installation of TinyXML2 development files
- Code generation for wxDataViewTreeCtrl sets an invalid style by default ([#799](https://github.com/wxFormBuilder/wxFormBuilder/issues/799))
- Error creating code in python. Create "true" Instead of "True" ([#803](https://github.com/wxFormBuilder/wxFormBuilder/issues/803))
- Generated C++ code cannot be built if wxNO_IMPLICIT_WXSTRING_ENCODING is defined ([#801](https://github.com/wxFormBuilder/wxFormBuilder/issues/801))
- Fix wxFlexGridSizer XRC output of growablerows and growablecols ([#806](https://github.com/wxFormBuilder/wxFormBuilder/pull/806))
- Lua: fixed missing parent name for top level box sizer ([#808](https://github.com/wxFormBuilder/wxFormBuilder/pull/808))

## [4.0.0] - 2023-11-14

### Added

- Component Search dialog
- DragAcceptFiles property
- Plugin SDK

### Changed

- Restructured Project Files layout
- Replaced library ticpp with TinyXML2
- Updated Plugin Interface for TinyXML2, this causes an API and ABI break
- Updated wxHtmlWindow event names for current wxPython API
- The Windows x64 Release now uses the MSYS2 64 Bit UCRT Runtime

### Removed

- Legacy Build System Meson
- Legacy Build System Premake
- Files supporting the legacy Build Systems

### Fixed

- Fixed unicode processing of the Template Processor
- Fixed incorrect use of API `wxAuiTabContainer::AddPage`
- Fixed generated code for Ribbon items
- Fixed wxButton like Widgets bitmap position parameter for XRC files
- Fixed wxSpinCtrlDouble properties min and max data type for XRC conversion
- Fixed wxBitmapComboBox constructor generation for wxPython
- Fixed wxRichTextCtrl crash on GNU/Linux
- Fixed GCC 13 build issues
- Fixed multiple timers callback bug
- Fixed wxAuiPaneInfo generation for wxToolBar and wxAuiToolBar components

## [3.10.1] - 2021-10-22

### Added

- Visit [GitHub](https://github.com/wxFormBuilder/wxFormBuilder/releases/tag/v3.10.1) for more information

## [3.10.0] - 2021-09-25

### Added

- Visit [GitHub](https://github.com/wxFormBuilder/wxFormBuilder/releases/tag/v3.10.0) for more information

### Fixed

- Too many to list :)

## 3.5.2 (RC2) - 2015-10-01

### Fixed

- #162 Setting extra style interferes with base class
- #159 Generated code for wxDataViewCtrl & co. is wrong/inconsistent
- #164 Alignment issues 3.5-RC1
- #165 A Fatal Error Occurred
- #157 Setting min larger than max causes crash

## 3.5.1.1 (RC1) - 2015-06-17

### Fixed

- Updated installer scripts

## 3.5.1 (RC1) - 2015-03-27

### Added

- #61 wxDataViewCtrlColumn support
- #8 Support for wxAUI_NB_MIDDLE_CLICK_CLOSE (patch by FGI)

### Fixed

- #142 3.5-0 beta doesn't generates cpp/h files from command line (patch by Alexey Elizarov)
- #145 ObjectInspector sometimes reselects wrong item (patch by sodev)
- #146 Fixed codegeneration to use the static box of a wxStaticBoxSizer as parent for its children (patch by sodev)
- #153 Incorrect generated python eventbinding code
- #7 get wxformbuilder to build on Mac OS X
- Partial fix of #131 wxStaticBoxSizer highlighting issues
- #9 fix mac os install scripts (patch by Roberto Perpuly)
- #10 update line endings in code generation on Mac OS X (patch by Roberto Perpuly)
- #121 Incorrect line endings for newer versions of Mac OS

## 3.5.0 (Beta) - 2014-05-06

### Added

- Added support for wxRibbonBar and related classes
- Added support for wxDataViewCtrl and derived classes
- Added ability to define property grid's/manager's content
- Re-arranged widgets palettes

### Changed

- Property names used in code templates may be encapsulated by curly brackets (e.g. ${name})
  so they can be surrounded by the template's content without any extra white spaces now

### Fixed

- #140 GUI doesn't work after some controls added
- #112 Using wxListbook under MSW causes assertion on startup

## 3.4.3 (Beta) - 2014-04-13

### Added

- The object inspector now remembers name of the last modified property and tries to select it
  again when active widget is changed (patch by tuyanyi)

### Fixed

- Both wxToolBar and wxAuiToolBar now use GetId() function member of their tools to handle
  ID assigned to the tools automatically
- #138 Wrong Icons in GUI
- #137 XRC generation uses class "unknown" for search control
- #136 AUI pane captions don't get marked for translation

## 3.4.2 (Beta) - 2014-01-24

### Added

- Added ability to swap the editor and the object properties panels (patch by tuyanyi)

### Fixed

- #126 wxWidgets > 2.8 compile fix (patch by sodev)
- #127 Wrong parent passed to IComponent::OnCreated() (patch by sodev)
- #128 Object Inspector expansion fix (patch by sodev)
- #129 Support for statusbar plugins (patch by sodev)
- Avoid flickering on MS Windows

## 3.4.1 (Beta) - 2013-11-12

### Changed

- Removed borders from designer frame/dialog/panel when built with wxWidgets 2.9 or later
  under wxGTK

### Fixed

- Fixed build error with wxWidgets 3.0.0

## 3.4.0 (Beta) - 2013-05-10

### Added

- Added support for wxLUA code generation

## 3.3.5 (Beta) - 2013-03-21

### Added

- Added support for wxInfoBar widget ( when built with wxWidgets 2.9.4 and later )
- Added support for wxPropertyGrid and wxPropertyGridManager widgets ( when built with wxWidgets 2.9.4 and later )
- Added support for wxStyledTextCtrl widget ( when built with wxWidgets 2.9.4 and later )
- Added support for wxTimer non-visual object
- Added XRC code generation for wxAuiNotebook ( when built with wxWidgets 2.9.5 and later )
- Added support for bitmaps in XRC code generated for wxNotebook

### Fixed

- Fixed various issues when built with wxWidgets 2.9.x and later
- Fixed "default" font family in generated XRC under wxWidgets 2.9.x and later
- Fixed minor visual issues under wxWidgets 2.9.x and later

## 3.3.4 (Beta) - 2012-10-07

### Fixed

- ID: 3519419 - Create inherited class bracket error
- Fixed code generation for PHP wxSplitterWindow wxEVT_IDLE
- Fixed "scrollbar jump" in Object Inspector occurred if wxFB built with wxWidgets 2.9.x and later.
- ID: 3536076, 3536080 - Sub-class behavior changed 3.2 to 3.
- ID: 3556216 - Missing field \<scrollrate\> in generated XRC
- ID: 3530800 - wxTextCtrl "maxlength"
- ID: 3142678 - Adds 'public' to python class when doing 'subclass'

## 3.3.3 (Beta) - 2012-04-06

### Fixed

- ID: 3510480 - Subclass not displayed after reloading a project in 3.3.2
- ID: 3506972 - Generating Python code
- ID: 3148236 - Configure wxAuiManager + AUI-related properties aren't shown for widgets/forms where it has no sense
- Fixed code generation for wxToolBar/wxAuiToolBar if parent form is managed by AUI
- wxAdditions widgets (such are wxPropertyGrid, wxScintilla, etc.) can be placed directly to the AUI-managed frame as well
- Offers validators just for those controls where it has a sense
- Improved highlighting of widgets if AUI is used
- AUI notebook is used instead of FlatNotebook if wxFB built with wxWidgets 2.9.x and later
- Fixed various bugs if wxFB built with wxWidgets 2.9.x and later
- Fixed Python code generation for tool placed inside stand-alone toolbar

## 3.3.2 (Beta) - 2012-02-10

### Fixed

- ID: 3470209 - wxDIRCTRL_MULTIPLE Style missing for wxGenericDirCtrl
- ID: 3472032 - Error in svn trunk 3.3-1 beta version generating python code
- ID: 3480059 - Gettext, Unicode and wxPython
- ID: 3486641 - XRC import does not accept CENTRE attributes
- ID: 3482829 - Default font is targeted to the current machine
- ID: 3460028 - Offer "default" value for fonts
- ID: 3488348 - Newlines in wxStaticText
- ID: 3493539 - Generate Inherited Class deletes additional includes (c++)

## 3.3.1 (Beta) - 2012-01-17

### Changed

- Added support for custom resource and client wxArtProvider's IDs in bitmap properties - John Labenski
- Added ability to set default selection for wxComboBox/wxBitmapComboBox

### Fixed

- Fixed code generation for bitmaps provided by wxArtProvider - John Labenski
- ID: 3437910 - Fixed codegeneration for context menus assigned to subclassed controls
- ID: 3435478 - Fixed crash on loading icon from resource
- ID: 3432356 - Possible bug with generated code for wxGridBagSizer
- ID: 3396919 - Bug on AUI toolbar buttons
- ID: 3395468 - wxChoice missing wxCB_SORT style
- ID: 3432383 - Missing delete for wxMenu object
- ID: 3417877 - Generated cpp file leads to compiler warnings
- ID: 3462454 - Fixed wxListCtrl code generation for wxEVT_COMMAND_LIST_COL_RIGHT_CLICK event
- wxFB source code can be built with wxWidgets 2.9.2 - Andrea Zanellato, Eran Ifrah

## 3.3.0 (Beta) - 2011-12-02

### Added

- Added support for wxWizard - Andrea Zanellato
- Added support for PHP code generation - JGM

### Changed

- Added missing IDs to wxArtProvider bitmap source
- Combo box component (wxComboBox) now supports value change from visual designer
- Drop-down menu associated with AUI toolbar's tool now shows its real content in designer
- Context menu associated with a widget is shown in designer after RMB click

### Fixed

- Fixed improper placement of a comma at the end of the C/C++ enum list - Andrea Zanellato
- Fixed gridbag sizer so it can be added directly to any container without need of
  another underlying sizer
- Fixed AUI pane buttons
- Fixed AUI code generation for wxToolBar/wxAuiToolBar

## 3.2.3 (Beta) - 2011-11-08

### Added

- Added "Load From Embedded File" for bitmap properties
- Added support for wxArtProvider
- Added support for drop-down menus in wxAuiToolBar - Andrea Zanellato
- XRCID() macro can be used as a widget ID

### Changed

- Upgraded to use new wxPropertyGrid control
- wxFB project version increased to 1.11 due to different format of bitmap properties

### Fixed

- Several toolbars can be inserted directly into AUI-managed frames
- Added missing import for wx.aui.AuiToolBar class
- Fixed handling of "moveable" AUI property
- Fixed C++ code generation for drop-down menus assigned to AUI toolbar's tools

## 3.2.2 (Beta) - 2011-03-22

### Changed

- Improved and fixed components' inheritance definition
- Number of rows in wxGridSizer and wxFlexGridSizer is set to '0' by default which
  means that the correct rows number will be calculated by wxWidgets itself
- Also following widgets can be placed directly into AUI-managed frame:
  wxNotebook, wxAuiNotebook, wxFlatNotebook, wxListbook, wxChoicebook, wxTreeListCtrl
  and wxSplitterWindow.
- AUI-managed frames can handle also AUI events
- Added "best_size", "min_size" and "max_size" AUI properties
- XRC preview is disabled for AUI-managed frames

### Fixed

- Fixed positioning of status bar on AUI-managed frame
- Fixed centering of frame/dialog which is managed by AUI
- Fixed doubled setup code for wxBitmapButton (and potentially for all components
  with multiple inheritance)
- wxGridBagSizer can be added directly to a frame/dialog/panel
- Fixed code generation of empty wxCheckListBox in Python

## 3.2.1 (Beta) - 2010-11-02

### Changed

- wxBitmapComboBox moved from "Additional" palette to "Common" palette

### Fixed

- Fixed occasional crash occurred during design of forms managed by AUI
- Fixed build issue on Linux when wxWidgets library was built with wxUSE_MEDIACTRL=0
  (build files must be re-created by using ./create_build_files.sh)
- Form/dialog background colour used in the designer is more "native"
- Fixed insertion of toolbars into AUI managed toolbar panes

## 3.2.0 (Beta) - 2010-09-17

### Added

- Added support for AUI and wxAuiToolbar
- Added support for wxBitmapComboBox and wxMediaCtrl

## 3.1.70 - 2010-07-31

### Changed

- Stable release for 3.1 branch

## 3.1.69 (RC2) - 2010-06-08

### Changed

- Slightly tuned the designer panel colour
- "internationalize" project property moved from C++ category to a common section

### Fixed

- Reverted patch for menu items' connection/disconnection. wxMenuItem::GetId()
  function in now used again instead of explicitly defined IDs. Explicitly defined
  IDs are now used for disconnection only.

## 3.1.68 (RC1) - 2010-04-05

### Added

- Generation of inherited class (F6) preserves all user code changes now (so it can be used
  more than once) (spedgenius)
- Any widget (wxWindow) can be added also to ToolBarForm (stand-alone toolbar class)
- Implemented internationalization support (gettext) in Python code

### Changed

- Searching scope of find dialog reduced to active editor page only to avoid confusing messages
  like multiple "not found"
- Slightly changed some main frame's components colors to better respect colours used in OS theme

### Fixed

- FS#456 - App crashes when you double click on file_path property
- FS#459 - duplicate python code generated on toolbar add widget
- Fixed disconnection of menu items' event handlers
- Fixed positioning of newly created items in object tree
- User-defined wrapping of static text is correctly generated also for XRC
- FS#436 - Relative path in Linux when under a symlink not generating correctly when .xpm included

## 3.1.67 (Beta) - 2010-02-10

### Changed

- Context menus are now generated as top-level menus in XRC so function wxXmlResource::LoadMenu()
  can be used on them
- Implemented XRC preview (F5) for stand-alone MenuBar and ToolBar

### Fixed

- Fixed resolving of name conflicts in stand-alone toolbars and menubars
- FS#454 - Python code generation for toolbars
- Fixed Python code generation of SetToolTipString() function for wxPanel

## 3.1.66 (Beta) - 2009-12-17

### Fixed

- Fixed python code generation for wx.EVT_LIST_ITEM_DESELECTED event (wxListCtrl)
- FS#420 - Minsize field is not generated
- FS#447 - Python code generation for ToolBar
- FS#449 - Python code generation for Menu: wrong id in Bind
- Fixed crash occurred when non-top level window was added directly to the project

## 3.1.65 (Beta) - 2009-08-27

### Added

- Implemented context menus for forms, containers and widgets. An instance
  of wxMenu class can be now assigned directly to these objects. The context menu
  is displayed on a right-mouse-button click by default, but the behavior can be
  changed by "context_menu" widget's option. Note that this feature is supported
  in C++ and Python code generation only.
- Menubar and toolbar can be inserted into project as top-level objects (they will
  be generated as stand-alone classes which can be instantiated later or directly into XRC).
  See 'Forms' palette.

### Changed

- Significantly improved speed of adding/removing of project items on large projects
- Significantly improved speed of undo/redo/paste operations on large projects (note for existing projects:
  the patch will have an effect after manual expansion and collapsing of all form-based items in the object tree)

### Fixed

- Fixed menu items' shortcuts in XRC
- FS#280 - wxMenu not part of a wxMenuBar
- Fixed startup crash on MacOS X 10.5.7 (for wxWidgets 2.8.10)
- Fixed loading of wxTreeListCtrl events that were inherited from wxTreeCtrl in
  wxAdditions
- FS#444 - python code generation error
- FS#428 - extra import(s) in wxpython code generation
- FS#446 - Can't add Submenu to a Menu

## 3.1.64 (Beta) - 2009-08-27

### Fixed

- FS#389 - Center option for dialogs should be set to wxBOTH by default
- FS#391 - Change (some) user notifications from status bar to message box

## 3.1.63 (Beta) - 2009-08-26

### Fixed

- Fixed code generation of wxGrid events in Python

## 3.1.62 (Beta) - 2009-08-19

### Added

- Added support for wxSearchCtrl widget
- Added support for wxAnimationCtrl widget

### Fixed

- Fixed code generation of system colors in Python
- FS#430 - Spacer code generation in Python is incomplete (no proportion param)
- FS#432 - Fatal error when resizing Object Properties
- Fixed tooltips' code generation in Python
- Sash position of wxSplitterWindow is set only if differs from -1

## 3.1.61 (Beta) - 2009-06-11

### Added

- Added support for Python (wxPython) code generation

### Fixed

- FS#424 - No encoding declared
- FS#425 - ListBook: C++ code instead of Python code

## 3.1.60 (Beta) - 2009-03-06

### Fixed

- Moved the 'event_handler' type to the Forms plug-in. This allows different
  event handling methods to be used per Frame/Dialog/Panel. This makes it even
  easier to create Dialogs that don't need any inherited class to work.
- Removed the 'virtual_decl' 'event_handler' method because it seemed like hassle
  to get a linker error instead of a compiler error. It also cleans up the options
  a bit.

## 3.1.59 (Beta) - 2009-02-06

### Fixed

- FS#367 - Project-Properties: encoding is ignored. Fixed ANSII file generation - Loaden
- FS#357 - wxFB overrides file without confirmation from user. - hykwok
- FS#215 - Abstract or normal class generation. - c16
- FS#394 - Separator reference not accessible through generated C++ code - pmendl
- FS#379 - Un/escaped some property values in XRC - malcom

## 3.1.58 (Beta) - 2008-08-04

### Added

- Added validator support for C++

## 3.0.57 - 2008-04-15

### Fixed

- FS#349 - "RC8 menu editor still generate incorrect menu item code"
- FS#345 - "Crash on Mac OS X when loading plugins"
- Fixed crash on close when showing a propgrid from the wxAdditions plugin
  in the Designer window
- FS#351 - "Can't edit choices if the '\' is used"

## 3.0.56 (RC8) - 2008-03-18

### Changed

- Removed the two project-level XRC properties, "bitmaps" and "icons".
  These never worked properly, and wxFB is not the right place to enter that information,
  a standalone XRC file written with any text-editor would be much better.
  This was prompted by, and caused the closure of, these issues:
  FS#340 - "bitmap resources are unnamed, preventing use of wxXmlResource::Get()->LoadBitmap()"
  FS#329 - "XRC Properties - Follow on"

### Fixed

- Added #include \<wx/intl.h\> to header file when using internationalization
- Custom control now inherits from wxWindow, to provide common properties
- FS#336 - "Incorrect ScrollBar Disconnection"
- FS#310 - "FB should not try to disconnect event handlers in dialog/frame's destructor."
  This is now optional, see the "disconnect_events" property of the Project object.
- FS#337 - "Incorrect Vertical Toolbar Rendering"
- FS#328 - "Unknown style flag wxTE_BESTWRAP for wxTextCtrl"
  Removed the flag, it is the default, and #define'd to 0x0000, so it was pointless anyway.
- FS#339 - "wx/icon.h not included when using resource icons in wxToolBar"
- FS#324 - "\[wxMac\] Buttons on the component palette have a black background"

## 3.0.55 (RC7) - 2008-02-18

### Fixed

- FS#319 - "Problems with Non-ASCII characters"
- FS#318 - "wxChoice window_name not generated"
- FS#314 - "wxListBox + Clicking on wxLC_VIRTUAL causes infinite loop"
- FS#317 - "wxFB 3.0 RC6 doesn't show wxStaticBitmap when added (? is shown)"

## 3.0.54 (RC6) - 2008-02-01

### Fixed

- FS#308 - Bugs in non-interactive (-g) mode
  Note: stderr is not normally available on Windows for GUI apps.
        If -g is used, all logging goes to stderr and debug output.
        Either start the process with a pipe for stderr (most IDEs will do this
        for you), or use a Debug Viewer to watch the output.
- FS#311 - wxFB 3.0 RC5 still generating incorrect wxMenu(item) code

## 3.0.53 (RC5) - 2008-01-15

### Changed

- Improved refresh of selection inside a scrolled window

### Fixed

- Fixed missing C++ code generation for menu separators and menu item bitmaps.
- FS#290 - "Toolbarseparator leads to crash if the Bitmapsize differs from default."
- Fixed crash on xrc generation for empty bitmap properties
- FS#295 - "SashGravity not exported in XRC"
- FS#297 - "wxColourPicker not selectable on form"
- FS#298 - "wxFontPickerCtrl not selectable on form"
- FS#299 - "wxFilePickerCtrl not selectable on form"
- FS#300 - "wxDirPickerCtrl not selectable on form"
- FS#301 - "wxToggleButton value property not updated"
- Fixed C++ generation of the wxToggleButton "value" property
- FS#302 - "wxSpinCtrl initial property not updating"
- FS#294 - "wxRadioBox not selectable on form"

## 3.0.52 (RC4-1) - 2007-12-17

### Fixed

- Fixed application closing when double clicking on an .fpb file in Windows
- FS#288 - \[OS X\] Splash screen is truncated
- FS#287 - Made wxFormBuilder compile on systems where wxStackWalker is not present
- FS#289 - \[OS X\] Perform file associations on mac

## 3.0.51 (RC4) - 2007-12-14

### Added

- FS#257 - "wxStaticBox inside a wxStaticBoxSizer lacks an id"
- Added stack trace dialog on fatal error

### Changed

- No longer creating a temporary file on disk for the XRC Preview

### Fixed

- FS#247 - "Invalid macro generated by wxFB."
- FS#248 - "Property choices of wxChoice: Strings with single quotes are truncated"
  This forced incrementing the .fbp version to 1.9
- FS#249 - "Paste From Clipboard requires Administrator privileges on Vista"
- FS#252 - "OnSize event bug in RC3 for wxPanel."
- FS#250 - "Image File Browser default filter should be "All Image Files""
- FS#251 - "UNC paths for images are not supported."
- FS#254 - "code duplicating."
- FS#261 - "wxMenuItem has no 'permission' property"
- FS#265 - "wxID_PREFERENCES should not be redefined"
- FS#269 - "\[OS X\] menu items shown at wrong place in menu bar" (patch from Auria)
- FS#263 - "Frame not resized correctly in designer when menubar/toolbar/statusbar present"
- FS#267 - "\[OS X\] radio button groups cause crash"
- FS#276 - "Creation of two event handlers with different prototypes" (patch from Joel Low)
- FS#279 - "missing Append() for wxMenuItem" (patch from denk_mal)
- FS#262 - "Centering of Frame/Dialog incorrect when frame/dialog size not pre-defined"
- FS#258 - "wxRadioButton enabled value not respected" (patch from Joel Low)
- FS#256 - "Generating Inherited class in different folder output malformed code"
- FS#268 - "\[OS X\] crashes upon termination" (patch from Auria)
- FS#277 - "crash on using icon/bitmap resource"
- FS#266 - "loading toolbar images from resource"
- FS#259 - "Toolbar tools' icon code does not respect toolbar size"
- FS#282 - "empty clipboard after closing wxformbuilder"
- FS#285 - "Whitespace event handler causes bad code"
- FS#283 - "possibility to set the selection of a wxChoice"

## 3.0.50 (RC3) - 2007-09-27

### Changed

- Common Events are now shared by all controls
- Auto sash positioning can now be disabled by setting /mainframe/AutoSash=0 in the config
- Enabled live pane update when dragging sashes
- All paths are now generated with '/' as a separator
- FS#246 - "Header file should be written to disk only if changed."

### Fixed

- FS#234 - "wxFrame name parameter not available."
- Property values are no longer trimmed for whitespace when projects are loaded
- FS#235 - "Events don't (always) get attached to the proper object."
- Now clearing the dirty flag when all changes are undone
- Cleaned up the selection of objects on undo/redo
- FS#236 - "include path for XPM Data incorrect."
- Secondary bitmaps of wxBitmapButton are now only generated to XRC if they are used
- The "select" property of notebook pages now corresponds to "selected" in XRC
- Improved XRC import:
  - Sizers are inserted if necessary and possible
  - Properties are now imported correctly where they used to fail in rare cases
  - New and informative warnings if an object cannot be imported
- FS#57 - "Can't add toolbar to non-frame containers."
- FS#238 - "Crash on Negative Number to growable* properties."
- FS#239 - "button.h not included when adding StdDialogButtonSizer."
- On Windows, Ctrl+C did not copy text from code editor when an object was not selected
- FS#241 - "\<wx/statbox.h\> not included in generated header."
- Removed minimum size from code windows so the tabs and scrollbars no longer disappear as the window is shrinking
- FS#242 - "wxFB does not respect current working directory."

## 3.0.49 (RC2) - 2007-09-06

### Fixed

- wxStdDialogButtonSizer events now Connect() to the button, instead of the parent
- Fixed "default" property for wxButton and wxBitmapButton
- Fixed missing bitmap properties on wxBitmapButton
- Applied patch from Ho Yin Kwok to check if the clipboard is open before opening it
- Fixed hang if opened with a project of an older version
- Fixed the find dialog only working one time
- Fixed FS#230 - "Repainting issue with border drawn by wxFormBuilder"
- The value of the "precompiled_header" property should now be the exact code to be
  generated at the top of the source file to support precompiled headers. For example,
  to include wxprec.h, the value of this property should be:  #include \<wx/wxprec.h\>
  This forced a project version uprev to 1.8.
  This closes FS#232 - "Precompiled headers with Visual C++ 2005".
- Removed #include \<wx/wx.h\> from the generated header

## 3.0.48 (RC1) - 2007-08-21

### Added

- FS#227 - "Custom control class name"
- Added wxGenericDirCtrl
- FS#173 - "Generate event handler stubs in Tools/Generate Inherited Class"

## 3.0.47 (Beta 3) - 2007-08-16

### Added

- FS#223 - "missing wxTextCtrl style"
- FS#222 - "Add an annoying dialog to the XRC Preview."

### Changed

- FS#116 - "Having property editor remember its expanded property groups."
- FS#218 - "Having file dialogs remember their type-selection, e.g. \*.png or \*.bmp"
- Added "All image files" to the file type filter for the file browser for
  bitmap properties

### Fixed

- Switch to Code::Blocks splashscreen class to fix linux transparency.
  This resolves FS#224 - "Splashscreen problem in Linux".
- FS#225 - "Current Radio Button selection not saved to fbp or generated in code."

## 3.0.46 (Beta 3) - 2007-07-27

### Added

- Added "column_sizes" and "row_sizes" properties to wxGrid.
  Resizing the columns/rows by dragging is the easiest way to use these.
- Added events to wxStdDialogButtonSizer
- Added wxAuiNotebook
- FS#214 - "Create icon for wxAuiNotebook"

### Changed

- Reorganized wxGrid properties
- Massive font update.
    Default values for font properties are now allowed and encouraged.
    The "underline" and "family" subproperties are now recognized.
    Fixed FS#205 - "wxStaticText If you bold a font and then remove the bold, the size shows up wrong"
    Watch for bugs.
- Applied patch from ChrisBorgolte which removes the need for wx2.6 compatibility mode.
    wxTHICK_FRAME and wxNO_3D styles have been removed from wxDialog. The project conversion wizard
    will take appropriate action.
- Updated images in sample text in wxRichTextCtrl. This resolves FS#221.

### Fixed

- Fixed FS#199 - "tooltips lost on XRC import"
- Fixed import of "enabled", "hidden", and "subclass" properties
- Fixed "FS#208 - Error in generated OnUpdateUI event handler registration code."
- Fixed "Edit->Paste Object From Clipboard" was not enabled until the next selection.
- Fixed  FS#216 - "missing header file for wxDatePickerCtrl"
- Properties with floating-point values will be saved and generated using the decimal point,
  but user entry will use the separator ('.' or ',') according to the locale settings.
  This closes "FS#203 - sashgravity -> float".
- Added "FS#211 - Optimized code/xrc generation in preview editors"

## 3.0.45 (Beta 3) - 2007-06-13

### Added

- Added support for context-sensitive help.
  To Use:
  Set the Project's "help_provider" property to one of the help providers listed.
  Set the "context_help" wxWindow property on any widget.
  wxMSW: Use the wxDIALOG_EX_CONTEXTHELP extra style on the dialog or frame.
  Others: Add a wxContextHelpButton by using wxStdDialogButtonSizer.

### Changed

- Updated the about dialog to include more details about the people involved in
  the project

### Fixed

- Fixed FS#198 - "Wrong place of the ID in a wxHyperlinkCtrl."

## 3.0.44 (Beta 3) - 2007-06-06

### Added

- Added FS#179 - "OnPaint for wxPanel."
- Added wxStaticText::Wrap()
- Implemented FS#192 - "Add support for dynamic event handling via Connect() methods."
  Connect() is now the default implementation of event handling.
  See the "event_generation" property of the Project object.
- Added CustomControl - This allows the user to generate arbitrary code for a
  control which is not yet fully supported by wxFormBuilder
- Added wxHyperlinkCtrl

### Changed

- Updated a bunch of icons. This completes the new theme in wxFormBuilder.

### Fixed

- Fixed FS#193 - "Missing wxTE_PROCESS_ENTER in style of wxComboBox."
- Fixed FS#178 - "crash using toolbar"
- Fixed sizeritem properties updating in property grid when modified via toolbar

## 3.0.43 (Beta 3) - 2007-05-29

### Added

- Implemented FS#185 - "Allowing access to wxStdDialogButtonSizer buttons."
- Added many more properties to wxGrid.
  This includes FS#191 - "Editing wxGrid column properties in the designer."
- Fixed xrc import of sizeritem, stringlist properties, and wxStdDialogButtonSizer.
  This was a regresssion in version 3.00.40.

### Changed

- Updated to wxFlatNotebook 2.2 - building with wx2.6.x is no longer supported.
  All notebooks can now be configured by right-clicking in empty area of tab bar.

### Fixed

- Fixed FS#190 - "annoying warning with wxTR_HIDE_ROOT in wxTreeCtrl"

## 3.0.42 (Beta 3) - 2007-05-12

### Added

- Added Right-Click -> "Close Preview" to XRC Previews.
  Also much improved the effect of the ESC key in GTK.
- Added wxRichTextCtrl

## 3.0.41 (Beta 3) - 2007-05-11

### Fixed

- Fixed FS#176 - "Dialog/Frame preview always displays all close/minimize/maximize buttons"
  Could probably still use some tweaking for the proper behavior in GTK.
- Fixed FS#179 - "OnPaint for wxPanel"
  Moved the OnPaint and OnBackgroundErase events to the common events.
- Fixed the wildly inefficient code for choosing the platform's EOL character.
  Significantly improves code generation (to file) time for larger projects.

## 3.0.40 (Beta 3) - 2007-05-04

### Added

- Implemented FS#53 - "Copy/Paste between instances of wxFB"
  See "Copy Object To Clipboard" and "Paste Object from Clipboard" in the "Edit" menu.
  Note: You can copy an object from wxFB and paste it as xml wherever you can paste text.
- Implemented FS#129 - "Controling control alignment from the keyboard."
- Implemented FS#135 - "Disabling the delete (Ctrl+D) option in the project's popup menu."

### Changed

- Replaced all code using TiXml classes with ticpp classes.
  WARNING: Although this has been tested, it could cause unforseen bugs. Please report all strangeness.

## 3.0.39 (Beta 3) - 2007-04-30

### Added

- Implemented FS#110 - "Drag & dropping objects in the Object Tree window."
  Holding CTRL down will copy the object instead of moving it.
- Implemented FS#10 - "add support for sizers as member variables"

### Fixed

- Fixed code generation for wxChoice, wxRadioBox, and wxCheckListBox for
  when the "choices" property is empty

## 3.0.38 (Beta 3) - 2007-04-18

### Changed

- Updated the program icon

### Fixed

- Fixed FS#169 - "About dialog box fails to open web browser for home page URL"
- Fixed FS#171 - "Wrong string literal macro in generated headers
  for internationalized code generation"
- Fixed FS#165 - "focus in wxNotebook always switches to the first panel after
  adding a widget/layout to another panel"
- Fixed FS#166 - "Adding three tabs to tab control throws an error."
- Fixed FS#172 - "Event list for wxListCtrl shows "OnListCasheHint" instead of "OnListCacheHint""
- Fixed crash when closing the Find dialog with the Cancel button
- Fixed FS#167 - ""Object Properties" splitter position not remembered"

## 3.0.37 (Beta 3) - 2007-03-27

### Added

- Added mouse events to common events
- Added wxEVT_ERASE_BACKGROUND to wxTopLevelWindow events

## 3.0.36 (Beta 3) - 2007-03-26

### Added

- Added style wxLC_NO_SORT_HEADER to wxListCtrl
- Added XRC support to wxFlatNotebook
- Added wxFontPickerCtrl
- Added wxFilePickerCtrl
- Added wxDirPickerCtrl
- Added OnIconize to wxTopLevelWindow events

### Changed

- Added a warning for unsupported properties
- Moved all container controls to a new containers plug-in
- Moved wxGrid to the "Additional" tab
- Moved wxRadioButton to the "Common" tab
- Moved wxPanel to the new "Containers" tab
- Updated most of the "Additional" tabs icons
- Reorganized the "Additional" tab

### Fixed

- Fixed FS#99 - "changes from menu editor remove all handlers."

## 3.0.35 (Beta 3) - 2007-03-19

### Added

- Added wxColourPickerCtrl using source provided by vaius

## 3.0.34 (Beta 3) - 2007-03-15

### Added

- Added extra_style property to xrc. This resolves FS#163.

### Changed

- Changed font size for xrc editor in gtk to match cpp editors
- Editors no longer move horizontal scrollbar when generating
- Added a mini wxFormBuilder used controls plug-in.
  It is disabled until a user downloads the wxAdditions plug-in
  from the website. (`http://wxformbuilder.org/?page_id=30`)
- Now using wxWidgets v2.8.2

### Fixed

- Updated propgrid, which fixed FS#162 - "Changing font point
  size in properties panel causes font size to become huge."

## 3.0.33 (Beta 3) - 2007-03-09

### Added

- Implemented FS#156 - "Ability to wrap generated code in namespace(s)"
  using patch supplied by kramar
- Implemented FS#157 - "Ability to decorate the generated class with dll export"
  using patch supplied by kramar
- Implemented FS#160 - "Support for namespaces for subclass forward declarations"
  using patch supplied by kramar

### Changed

- Moved declaration of menus and submenus to header file

### Fixed

- Fixed FS#161 - "Tag of items in wxCheckListBox."

## 3.0.32 (Beta 3) - 2007-03-07

### Changed

- New way to save propgrid values when using menu or toolbar items.
  Cleaner and cross-platform.

## 3.0.31 (Beta 3) - 2007-03-06

### Changed

- Implemented FS#138 - "Select the 'next' control after deleting a control."

### Fixed

- Fixed FS#149 - "Double question whether you want to save a project on exit."
- Fixed FS#148 - "Different generated C++ and XRC file names for unnamed projects."
- Fixed FS#154 - "Crash when adding a sizer."
- Fixed FS#147 - "Accepting new property values when clicking on a menu/toolbar item."

## 3.0.30 (Beta 3) - 2007-03-05

### Changed

- Implemented FS#130 - "Support for searching through XRC or C++ code."

## 3.0.29 (Beta 3) - 2007-03-04

### Added

- FS#146 - "Displaying keyboard shortcuts in toolbar tooltip windows."
- New keyboard shortcut for 'New Project' Ctrl+N

### Changed

- Updated the keyboard shortcut for 'Save As' from "F3" to "Ctrl-Shift+S"
- Updated the keyboard shortcut for 'Open' from "F2" to "Ctrl+O"

## 3.0.28 (Beta 3) - 2007-03-03

### Fixed

- Fixed FS#143 - "Silent code generation failure in case of an invalid project path."
  This was a regression in Version 3.00.27.
- Fixed FS#118 - "Code not regenerated on property changes."
- Fixed FS#119 - "Generating files should not force the code display control to scroll back to the top."
- Code in the visible panel is now regenerated on object removal and creation, project load, new project
  and event handler modification.
- Fixed FS#144 - "Incorrectly generated C++ code when precompiled_header
  property was empty."

## 3.0.27 (Beta 3) - 2007-03-02

### Changed

- Implemented FS#142 - "set wxStaticline's default value to wxEXPAND"
- Implemented FS#140 - "Better code generation selection implementation."
  Always generate code to the visible panel.
  Added -l (language) command line option to specify languages to generate
  when generating from command line. Separate multiple languages with commas.

## 3.0.26 (Beta 3) - 2007-03-01

### Changed

- Implemented FS#117 - "ESC should close the XRC preview form."
  Note: This will never work on wxGTK because it does not set key events to frames or dialogs.

### Fixed

- Fixed FS#136 - "Panel size doesn't change properly when 'size' is set to -1"
- Fixed FS#137 - "Allow -1 for the 'size' property if the minimum/maximum size is set."
- Fixed FS#113 - "Error when undoing cutting controls."
- Fixed FS#97 - "Menu items in XRC preview window involve wxFB functionality"

## 3.0.25 (Beta 3) - 2007-02-28

### Changed

- Now connecting an event handler to idle events for splitter windows to reliably
  set the initial sash position

### Fixed

- Fixed FS#100 - "Problem with the relative path of XPM"
- Fixed FS#126 - "Splitter window control without a panel does not get redrawn correctly."
- Fixed FS#132 - "Splitter window control moves its sash unexpectedly at design time."
- Fixed spacer not displaying properly in most sizers.
  This was a regression in build 3.00.23.
- Fixed FS#133 - "Error when moving between designer/C++/XRC tabs."
  This was a regression in build 3.00.24.

## 3.0.24 (Beta 3) - 2007-02-27

### Fixed

- Fixed FS#131 - "Folder not getting released after saving a project into it."
- Fixed FS#114 - "Not being able to cut non-top-level sizers and paste them as top-level sizers."
- Fixed FS#124 - "Option to create a sizer around a control does not work on top-level sizers."
- Fixed FS#104 - "Editing XRC/C++ files should be disallowed completely or allowed to affect GUI properties."
- Fixed FS#127 - "Change 'new' label to 'new project'."
- Fixed FS#125 - "Buggy cut/paste of panels from splitter window controls."

## 3.0.23 (Beta 3) - 2007-02-26

### Added

- Added wxGridBagSizer. This closes FS#58.
  Cannot use wxGBS as the first sizer in a form until the object model improves.

### Changed

- Spacer now uses sizeritem, instead of being standalone.
  This forced a project version uprev to 1.6
- Converted all wxLogDebug to Debug::Print so __WXFB_DEBUG__ is used to
  determine whether or not to log
- Improved precompiled header support.
  NOTE: You must now define WX_PRECOMP when compiling to use a precompiled header.

## 3.0.22 (Beta 3) - 2007-02-23

### Added

- Added most of the higher level events for wxTopLevelWindows and wxWindow

### Changed

- Updated the event layout and used event inhertitance to get common events
  throughout the controls

## 3.0.21 (Beta 3) - 2007-02-22

### Added

- Added the 'Additional' controls events
- Added wxEVT_UPDATE_UI event to all controls (if it has a sense)

### Fixed

- Prevent event handler duplication when there are different event handlers
  with the same function (but it doesn't check that the event class is the
  same)
- Fixed FS#98 - Non-existing files are removed from the recent files list

## 3.0.20 (Beta 3) - 2007-02-18

### Changed

- "View:XRC Window" now only generates the form that it will display

### Fixed

- Fixed FS#93 - "wxGrid -> Object type unknown in XRC."
- Fixed FS#94 - "Typo in code generation which includes \<wx/imaglist.h\>."
- Fixed FS#96 - "No XRC for wxStdDialogButtonSizer."
- Added wxRadioBox label to XRC output

## 3.0.19 (Beta 3) - 2007-02-12

### Changed

- wxFrame title bar color now using ActiveCaption

### Fixed

- Fixed FS#89 - "wxSpinCtrl cannot have negative min/max value."
- Fixed FS#90 - "Can't change the source of a bitmap."
- Fixed FS#91 - "ID's that start with 'wx' are not properly generated."
- MainFrame is updated when a property is changed, even if the object
  doesn't match with the selected object (i.e. sizeritem obj)

## 3.0.18 (Beta 3) - 2006-02-01

### Added

- Implemented FS#6 - "icons and bitmaps support"
- Implemented FS#87 - "Run as command-line compiler"

### Fixed

- Undo command of MoveHierarchy (move left) now restores the position of
  the object
- Fixed FS#86 - "Object-tree context-menu "Copy" command has no effect"

## 3.0.17 (Beta 2) - 2007-01-19

### Added

- Added new 'Generate Inherited Class' dialog
- Added menu events
- Added Toolbar tool events

### Changed

- Changed generation of "minimum_size" and "maximum_size" for wxFrame and
  wxDialog to using SetSizeHints

### Fixed

- Fixed bug where dragging on created dialog statusbar was resizing the main
  application window
- Fixed FS#85 - "required wx/imagelist.h for wxListbook using image files."
  with help from akira
- Fixed the "relative_path" and "internationalize" properties for code
  generated from the inner template of a macro. This resolves FS#83 and
  FS#84.

## 3.0.16 (Beta 2) - 2007-01-11

### Added

- Added "kind" property to tools and added "tooltip" and "statusbar"
  properties to the generated XRC.
  This resovles FS#79 - "Toggle buttons on toolbar."

### Changed

- Updated the splash screen again

### Fixed

- Improved tolerance of wxBitmap properties to old project files.
  This resolves FS#78.

## 3.0.15 (Beta 2) - 2007-01-09

### Added

- Added new splash screen

### Fixed

- Fixed a focus bug that crashed when adding a sizer in certain container
  widgets

## 3.0.14 (Beta 2) - 2007-01-04

### Added

- Added all known events to the common tab
- Added descriptions of the subproperties for bitmap properties
- Added support for using the non-default editors in the property grid.
  Just add 'editor="Name"' to your xml element for the property.
- Added a warning about the removal of the "user_headers" property during
  the project import process
- The selection border can be drawn even when it extends beyond the edge of
  the inner content panel
- Catching left clicks on the title bar of innerframe in order to select it
- Added wxSL_BOTH style flag to wxSlider
- Added encoding check for all loaded xml files. The means that the xml files
  must have a declaration, and the encoding must be UTF-8. If it is not, wxFB
  will offer to convert the file, from a user chosen original encoding. This
  resolves FS#69.
- Add the .fbp extension when saving if none is chosen. (this is really only
  an linux issue)
- Added a shortcut to the "XRC Window" menu item
- Added property to skip frame's main sizer generation in certain cases for xrc code
- Now using ints instead of uints for min, max, and val on wxSlider
- Now remembering right splitter size in classic gui
- Fixed wxSlider XRC generation for the minValue and maxValue properties
- Status bar now stays on the bottom of the frame even when the sizer is empty

### Changed

- Single Instance Stuff:
    Prefixed lockfiles with '.' so they are hidden.
    Switched using "localhost" to "127.0.0.1", "localhost" was not working for me.
    Added a status bar message in the second instance when the first instance
    is brought to the front.
    Moved single instance stuff to wxfbipc.cpp/h.
- General innerframe rendering updates

### Removed

- Removed SetVendorName, it just added a level to the hierarchy in the windows
  registry

## 3.0.13 (Beta 2) - 2006-12-22

### Fixed

- Now generating all empty strings as wxEmptyString. This resolves FS#52.
- wxRadioBox can now use internationalization on its label, too.
  This resolves FS#50.
- Generation of Microsoft BOM on UTF-8 files is now disabled on
  platforms other than WXMSW. This resolves FS#46.
- Fixed crash when editing the value of a wxTextCtrl in the designer,
  under certain conditions. This resolves FS#45.

## 3.0.12 (Beta 2) - 2006-12-08

### Fixed

- Fixed problems with paths including non-English characters.
  wxFB can now be installed to such paths, as well as open/save projects,
  generate code, and import xrc files. This resolves FS#49.
- Now only allowing one choice for non_flexible_grow_mode and for
  flexible_direction on wxFlexGridSizer. This resolves FS#56.

## 3.0.11 (Beta 2) - 2006-12-04

### Fixed

- Now generating user_headers include statements above the generated
  header include. This resolves FS#55.
- Fixed FS#51 - "notebook-like objects errors while they are included into each other"
- Fixed FS#59 - "wxCheckListBox: incorrect class name in XRC"
- Fixed  FS#62 - "Sub-Classing top level items doesn't work."
  This was a big change. The subclass property is now a parent property with two
  children: name and header. The name is the subclass name, and header is the
  header to include so that the subclass is defined while building. The header
  is placed into the correct generated file depending on whether a form or control
  is being subclassed. This removes most of the purposed of the "user_headers"
  property, so it has been removed. A "precompiled_header" property was added,
  with default value of "wx/wxprec.h". This fix necessitated the creation of a "parent"
  property type, of which "subclass" is the only example. More information on parent
  properties will soon be added to the wiki.

## 3.0.10 (Beta 2) - 2006-11-15

### Fixed

- Fixed FS#63 - "Size/minimum-size is not working."
- Fixed FS#60 - "wxFlexGridSizer vgap/hgap if empty they get generated empty."
  This was actually a bug with most numerical properties. Added a "uint" property
  type to solve this. Most numerical properties are now unsigned integers, so the
  property grid will not let it remain blank, and negative numbers are not accepted.
  This approach is good because it is clear to the user what will happen when the
  code is generated.
- Fixed FS#66 - "Adding common controls to toolbar."
- Fixed FS#65 - "Unfriendly object tree."
- Implemented FS#68 - "Smart enable/disable of toolbar items."
  This was already mostly done. Just needed to enable/disable the appropriate alignment
  buttons for wxBoxSizer and wxStaticBoxSizer.

## 3.0.9 (Beta 2) - 2006-10-30

### Fixed

- Fixed moving left, right, and into a new wxBoxSizer for spacers
- Fixed FS#72 - "When editing a property in the property grid, it is lost when you click away."
- Fixed FS#73 - "Toolbar tools not selecting the object in the object tree."
- Fixed FS#71 - "wxSplitter window sashpos not being set."

## 3.0.8 (Beta 2) - 2006-10-27

### Fixed

- ID_DEFAULT is no longer defined when not used
- Ids beginning with "wxID_" are no longer defined, this is to avoid redefining
  wxWidgets special identifiers
- Ids for forms now use the "id" property instead of being hardcoded to -1
- Fixed wxSlider XRC generation for the minValue and maxValue properties

## 3.0.7 (Beta 2) - 2006-10-26

### Changed

- wxID_ANY is now the default id
- Object tree now saves its state
- Object tree now also scrolls horizontally when selecting an object
- Added class name and line to error message for unknown classes when importing xrc
- Improved both C++ and XRC code generation for wxSplitterWindow

## 3.0.6 (Beta 2) - 2006-10-25

### Added

- Added support for using wxSplitterWindow with only one child
- Added min_pane_size and sashsize properties to wxSplitterWindow
- Added maximum size property to wxWindow properties

### Changed

- Implemented FS#26 - "Chronological id creation."

## 3.0.5 (Beta 2) - 2006-10-20

### Added

- Implemented FS#48 - "Improve error notification when a plugin is missing."
  If widgets are missing, the project will not load at all.
- Added a boolean "xrc_skip_sizer" property to Frame. The default is value is true.
  When true, the main sizer of the frame will not be generated in XRC, if the sizer
  only has one child. This is a workaround for a wx bug that ignores the "size"
  property of the frame if it has a sizer. This resolves FS#74.
- Added encoding check for all loaded xml files

## 3.0.4 (Beta 1) - 2006-10-13

### Changed

- Now limiting to a single instance of wxFB per project file. The resolves FS#31.

### Fixed

- Fixed visual editor for subpanels
- Fixed circular subclassing. This resolves FS#32.
- Fixed Alt-Tab icon. This resolves FS#34.

## 3.0.3 (Beta 1) - 2006-10-08

### Fixed

- Fixed wxWindow settings code generation

## 3.0.2 (Beta 1) - 2006-10-07

### Added

- Added center property for wxDialog and wxFrame. This closes FS#17.
- Individual directories for each plug-in
- Toolbar separators. This closes FS#11.
- Possibility to group plugin components with toolbar separators

### Changed

- Premake script for sdk projects
- Renamed contrib to wxAddition plug-in
- Palette now remembers tab order
- Major ripup/redesign of the plugin interface
- Plugin directory out of src
- wxAdditions plug-in to the wxAdditions repository

### Fixed

- Fixed generation bugs in submenu and wxCheckList (now wxCheckListBox)
  These were caused by subclass in version 2.00.75. This closes FS#20.
- Changed title of save dialog box from "Open Project" to "Save Project".
  This closes FS#18.
- Fixed xrc generation of static bitmap using patch supplied by henrique.
  This closes FS#16.
- Added subclassing on forms. This closes FS#21.
- Fixed C++ output of value property for wxGauge

## 2.0.75 - 2006-09-13

### Added

- Added subclass and user_headers properties. This closes FS#5.

## 2.0.74 - 2006-09-07

### Fixed

- Fixed behaviour of wxSize and wxPoint properties when NumLock was
  on (and possibly others)

## 2.0.73 - 2006-08-28

### Changed

- File size for wxAdditions in the wxFormBuilder installer

### Fixed

- Fixed behaviour of wxSizer::Fit(). This fixes FS#9.

## 2.0.72 - 2006-08-25

### Added

- Replaced wxPlot with wxPlotCtrl

### Changed

- Made checkbox the default for boolean properties
- Automatic linking to wxAdditions is now done through a
  header distributed with wxAddtions, this is now independent
  of the version of wxWidgets

### Removed

- Removed project option for wxWidgets Version

### Fixed

- Fixed xrc generation for bitmap properties (bug FS#4)

## 2.0.71 - 2006-08-18

### Added

- Added a new project option for wxWidgets Version,
  currently it can be 2.7+ and 2.6+
- Added support for wxWidgets v2.7.0 for code generation

## 2.0.70 - 2006-08-15

### Added

- Added wxScintilla shortcuts (Ctrl-C, Ctrl-V, Ctrl-X)
- Added "Default" in the wxPropertyGrid colour selector
  (some controls don't have the BtnFace colour by default)
- Added the properties checked and style to wxCheckBox

### Changed

- Set the default background color of wxFlatNotebook to ButtonFace
  Updated wxFB's URL in the generated C++ code

### Fixed

- Fixed the background colour of wxPanel
- Fixed bitmap behavior on menu items

## 2.0.69 - 2006-08-03

### Fixed

- Fixed bug where the first page of listbook, choicebook, and flatnotebook
  was always selected on refresh

## 2.0.68 - 2006-08-01

### Fixed

- Updated the executable in the installer. Opps ;-)

## 2.0.67 - 2006-08-01

### Added

- Option to make Microsoft BOM for UTF-8 files added to project settings

### Changed

- Updated icons to look a bit more modern

## 2.0.66 - 2006-07-27

### Fixed

- Fix XRC styles importation

## 2.0.65 - 2006-07-27

### Changed

- Made it so the unique names are created with numbers instead of '_'

## 2.0.64 - 2006-07-26

### Changed

- Made the wxAdditions installer do better checks to determine if the user
  has wxWidgets installed before deciding where the default directory will be
- Moved the download location for wxAdditions to a sourceforge hosted server

## 2.0.63 - 2006-07-25

### Changed

- Fixed a spelling mistake in the installer

## 2.0.62 - 2006-07-25

### Changed

- Added a description in the installer about wxAdditions

## 2.0.61 - 2006-07-24

### Changed

- Installer to dynamically download wxAdditions
- wxPropgrid v1.2

## 2.0.60 - 2006-07-21

### Changed

- Code generation for wxRadioBox and wxChoice improved

### Fixed

- Fixed Unicode support
- Fixed style not working in a frame and dialog

## 2.0.59 - 2006-07-14

### Added

- Added wxScintilla control to contrib

### Changed

- Installer Maintenance release

## 2.0.58 - 2006-07-14

### Added

- Replaced common icons with new look
- Project Conversion supported

### Fixed

- Fixed Unicode xrc output bug

## 2.0.57 - 2006-07-06

### Added

- Added the move right/left to the right-click menu

### Fixed

- Fixed wxMenuBar 'style' property

## 2.0.56 - 2006-06-28

### Fixed

- Fixed handling of default values for inherited properties

## 2.0.55 - 2006-06-27

### Added

- Added refresh call to object inspector panel
- Added code generation for the extra_style property

### Fixed

- Stopped combining bitlists with the same name
- Fixed wxPropertyGridManager disappearing-toolbar bug (Linux)

## 2.0.54 - 2006-06-22

### Added

- This changelog is in the installer

### Fixed

- UTF-8 file generation occurrs by default in the unicode build
- Changed the xrc to xml conversions to use the wxConvCurrent conversion

## 2.0.53 - 2006-06-21

### Added

- Support for wxScrolledWindow
- Can now set images on wxNotebook and wxFlatNotebook tabs
- Can now set images on wxListbook
- Control IDs can now be generated as an enumeration instead of a list of #defines
- Now supports internationalization
- Can now set the encoding on XRC files
- Now building in Unicode for better Cross-Platform support
- Now supports "flexible_direction" and "non_flexible_grow_mode" on wxFlexGridSizer
- Now supports Sort style on wxComboBox

### Removed

- Strange empty lines in generated code

### Fixed

- Message box titles now begin with a lowercase "w"

## 2.0.52 - 2006-06-05

### Added

- Support for wxListbook
- Support for wxChoicebook
- Added help in the GUI for all properties

## 2.0.51 - 2006-05-17

### Added

- Can now use images from windows .rc files for all image properties

## 2.0.50 - 2006-02-10

### Added

- Support for wxPlotWindow
- Support for wxPropertyGrid
- Support for wxPropertyGridManager
- Support for wxFlatNotebook
- Support for awxLed
- Support for wxStaticBoxSizer
- Icons to wxFlatnotebook tabs
- Folding to scintilla control
- Line numbers to scintilla control
- Minimum size property to wxWindow
- Code is genereted in the GUI when switching to the notebook page, without
  requiring a click of the "Code Generation" button

### Changed

- Toolbar icons
- Interface now using wxFlatNotebook
- Now using the latest propgrid source
- Interface now using propgrid manager
- Interface now using wxScintilla
- Change wxColor property to show a list of system colors
- All icons can now be customized from the xml files
- Change "option" to "proportion" on sizeritems
- Switch to tabs for indentation

### Removed

- Typedefs for easier understanding

### Fixed

- Background color issues in the palette

[unreleased]: https://github.com/wxFormBuilder/wxFormBuilder/compare/v4.1.0...HEAD
[4.1.0]: https://github.com/wxFormBuilder/wxFormBuilder/compare/v4.0.0...4.1.0
[4.0.0]: https://github.com/wxFormBuilder/wxFormBuilder/compare/v3.10.1...v4.0.0
[3.10.1]: https://github.com/wxFormBuilder/wxFormBuilder/compare/v3.10.0...v3.10.1
[3.10.0]: https://github.com/wxFormBuilder/wxFormBuilder/releases/tag/v3.10.0

<!-- markdownlint-configure-file { "no-duplicate-heading": { "siblings_only": true } } -->
