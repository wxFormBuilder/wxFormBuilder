;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; File:        wxFormBuilder.iss
; Author:      Ryan Pusztai & Ryan Mulder
; Date:        02/07/2006
; Copyright:   (c) 2007 Ryan Pusztai <rpusztai@gmail.com>
;              (c) 2007 Ryan Mulder <rjmyst3@gmail.com>
; License:     wxWindows license
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define MyAppVer "3.9.0"
#define MyAppName "wxFormBuilder"
#define MyAppPublisher "Jose Antonio Hurtado"
#define MyAppURL "http://wxformbuilder.org"
#define MyAppExeName "wxFormBuilder.exe"

[Setup]
AppName={#MyAppName}
AppVerName={#MyAppName} {#MyAppVer}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={commonpf}\{#MyAppName}
DisableDirPage=false
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=false
OutputBaseFilename={#MyAppName}-{#MyAppVer}
Compression=lzma/ultra
SolidCompression=true
InternalCompressLevel=ultra
OutputDir=.
ShowLanguageDialog=yes
AppVersion={#MyAppVer}
AppendDefaultGroupName=false
AllowNoIcons=true
WizardImageFile=compiler:WizModernImage-IS.bmp
WizardSmallImageFile=compiler:WizModernSmallImage-IS.bmp
SetupIconFile=support\wxFormBuilder.ico
UninstallDisplayIcon={app}\wxFormBuilder.exe
ChangesAssociations=true
VersionInfoVersion={#MyAppVer}
VersionInfoDescription={#MyAppName}
InfoAfterFile=..\..\output\Changelog.txt
LicenseFile=..\..\output\license.txt
MinVersion=0,6.0

[Messages]
BeveledLabel={#MyAppName} {#MyAppVer}

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[Files]
Source: ..\..\output\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs; Excludes: .svn\*, lib64, *d.exe, *d.dll, wxmsw30ud_*, wxmsw30umd_*, Thumbs.db, *.a
Source: C:\msys64\mingw32\bin\wx*.dll; DestDir: {app}
Source: C:\msys64\mingw32\bin\libstdc++*.dll; DestDir: {app}
Source: C:\msys64\mingw32\bin\libgcc*.dll; DestDir: {app}
Source: C:\msys64\mingw32\bin\libintl*.dll; DestDir: {app}
Source: C:\msys64\mingw32\bin\libexpat*.dll; DestDir: {app}
Source: C:\msys64\mingw32\bin\libjpeg*.dll; DestDir: {app}
Source: C:\msys64\mingw32\bin\libpng*.dll; DestDir: {app}
Source: C:\msys64\mingw32\bin\libtiff*.dll; DestDir: {app}
Source: C:\msys64\mingw32\bin\zlib*.dll; DestDir: {app}
Source: C:\msys64\mingw32\bin\libwinpthread*.dll; DestDir: {app}
Source: C:\msys64\mingw32\bin\libiconv*.dll; DestDir: {app}
Source: C:\msys64\mingw32\bin\liblzma*.dll; DestDir: {app}
Source: C:\msys64\mingw32\bin\libzstd*.dll; DestDir: {app}

[InstallDelete]
; Cleanup debug dlls.
Name: {app}\plugins\additional\libadditionald.dll; Type: files
Name: {app}\plugins\common\libcommond.dll; Type: files
Name: {app}\plugins\layout\liblayoutd.dll; Type: files

[Icons]
Name: {group}\{#MyAppName}; Filename: {app}\{#MyAppExeName}
;Name: {group}\{#MyAppName} Help; Filename: {app}\{#MyAppName}.hlp
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}
Name: {userdesktop}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: desktopicon

[Run]
Filename: {app}\{#MyAppExeName}; Description: {cm:LaunchProgram,{#MyAppName}}; Flags: nowait postinstall skipifsilent

[Components]
Name: main; Description: wxFormBuilder (required); Flags: fixed dontinheritcheck checkablealone; Types: custom compact full

[Registry]
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\{#MyAppExeName}; ValueType: string; ValueData: {app}\{#MyAppExeName}; Flags: uninsdeletekey
Root: HKCR; SubKey: .fbp; ValueType: string; ValueData: {#MyAppName}.Project; Flags: uninsdeletekey
Root: HKCR; SubKey: {#MyAppName}.Project\DefaultIcon; ValueType: string; ValueName: ; ValueData: {app}\{#MyAppExeName}; Flags: uninsdeletekey
Root: HKCR; SubKey: {#MyAppName}.Project; ValueType: string; ValueData: {#MyAppName} Project File; Flags: uninsdeletekey
Root: HKCR; SubKey: {#MyAppName}.Project\Shell\Open\Command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Flags: uninsdeletevalue
