;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; File:        wxFormBuilder.iss
; Author:      Ryan Pusztai & Ryan Mulder
; Date:        02/07/2006
; Copyright:   (c) 2007 Ryan Pusztai <rpusztai@gmail.com>
;              (c) 2007 Ryan Mulder <rjmyst3@gmail.com>
; License:     wxWindows license
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


; MyAppVer gets parsed from source file
#define MyAppName "wxFormBuilder"
#define MyAppPublisher "Jose Antonio Hurtado"
#define MyAppURL "http://wxformbuilder.org"
#define MyAppExeName "wxFormBuilder.exe"

; The #define Arch is expected to be supplied externally and must contain a MinGW-w64 architecture.
; If the architecture is 64 bit, enable 64 bit mode and enforce a 64 bit environment, otherwise
; assume 32 bit and run in default mode.
#define MyAppArchitecture
#define MySetupArchitecture
#ifdef Arch
  #if Arch == "x86_64"
    #define MyAppArchitecture "x64"
    #define MySetupArchitecture "-x64"
  #else
    #define MySetupArchitecture "-x86"
  #endif
#endif

#define protected FileHandle
#define protected FileLine

#sub ProcessVersionLine
  #define private FileLine = FileRead(FileHandle)
  #if Pos("VERSION = """, FileLine) > 0
    #define private temp Copy(FileLine, Pos("""", FileLine) + 1)
    #define public MyAppVer Copy(temp, 1, RPos("""", temp) - 1)
  #endif
#endsub

#for {FileHandle = FileOpen("..\..\_build\src\rad\version.cpp"); FileHandle && !FileEof(FileHandle) && !Defined(MyAppVer); ""} ProcessVersionLine
#if FileHandle
  #expr FileClose(FileHandle)
#endif


[Setup]
AppName={#MyAppName}
AppVersion={#MyAppVer}
AppVerName={#MyAppName} {#MyAppVer}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
VersionInfoVersion={#MyAppVer}
VersionInfoDescription={#MyAppName}
ArchitecturesAllowed={#MyAppArchitecture}
ArchitecturesInstallIn64BitMode={#MyAppArchitecture}
MinVersion=6.1sp1
OutputDir=.
OutputBaseFilename={#MyAppName}-{#MyAppVer}{#MySetupArchitecture}
Compression=lzma/ultra
SolidCompression=true
InternalCompressLevel=ultra
WizardStyle=modern
InfoAfterFile=..\..\_install\Changelog.txt
LicenseFile=..\..\_install\license.txt
SetupIconFile=support\wxFormBuilder.ico
ShowLanguageDialog=yes
DefaultDirName={commonpf}\{#MyAppName}
DisableDirPage=false
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=false
AppendDefaultGroupName=false
AllowNoIcons=true
ChangesAssociations=true
UninstallDisplayIcon={app}\wxFormBuilder.exe


[Types]
Name: "full"; Description: "Full installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom


[Components]
Name: main; Description: wxFormBuilder (required); Types: full custom; Flags: fixed checkablealone
Name: runtime; Description: Compiler Runtime (required); Types: full custom; Flags: fixed checkablealone


[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked


[Files]
#define protected RootDir "..\..\_install"
#define protected PluginsBaseDir "plugins"

#define protected FindHandle
#define protected FindResult

#sub ProcessFoundPlugin
  #define private FileName FindGetFileName(FindHandle)
  #if FileName != "." && FileName != ".."
    #define private PluginsSourceDir AddBackslash(RootDir) + AddBackslash(PluginsBaseDir) + FileName
    #define private PluginsDestDir AddBackslash(PluginsBaseDir) + FileName
    Source: {#PluginsSourceDir}\lib{#FileName}.dll; DestDir: {app}\{#PluginsDestDir}; Flags: ignoreversion; Components: main
    Source: {#PluginsSourceDir}\icons\*; DestDir: {app}\{#PluginsDestDir}\icons; Flags: recursesubdirs createallsubdirs; Components: main
    Source: {#PluginsSourceDir}\xml\*; DestDir: {app}\{#PluginsDestDir}\xml; Flags: recursesubdirs createallsubdirs; Components: main
  #endif
#endsub

Source: {#RootDir}\Changelog.txt; DestDir: {app}; Components: main
Source: {#RootDir}\license.txt; DestDir: {app}; Components: main
Source: {#RootDir}\wxFormBuilder.exe; DestDir: {app}; Flags: ignoreversion; Components: main
Source: {#RootDir}\resources\*; DestDir: {app}\resources; Flags: recursesubdirs createallsubdirs; Components: main
Source: {#RootDir}\xml\*; DestDir: {app}\xml; Flags: recursesubdirs createallsubdirs; Components: main
#for {FindHandle = FindResult = FindFirst(AddBackslash(RootDir) + AddBackslash(PluginsBaseDir) + "*", faDirectory); FindResult; FindResult = FindNext(FindHandle)} ProcessFoundPlugin
#if FindHandle
  #expr FindClose(FindHandle)
#endif

Source: {#RootDir}\wx*.dll; DestDir: {app}; Components: main

Source: {#RootDir}\lib*.dll; DestDir: {app}; Components: runtime
Source: {#RootDir}\zlib1.dll; DestDir: {app}; Components: runtime


[Icons]
Name: {group}\{#MyAppName}; Filename: {app}\{#MyAppExeName}
;Name: {group}\{#MyAppName} Help; Filename: {app}\{#MyAppName}.hlp
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}
Name: {autodesktop}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: desktopicon


[Registry]
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\{#MyAppExeName}; ValueType: string; ValueData: {app}\{#MyAppExeName}; Flags: uninsdeletekey
Root: HKCR; SubKey: .fbp; ValueType: string; ValueData: {#MyAppName}.Project; Flags: uninsdeletekey
Root: HKCR; SubKey: {#MyAppName}.Project\DefaultIcon; ValueType: string; ValueName: ; ValueData: {app}\{#MyAppExeName}; Flags: uninsdeletekey
Root: HKCR; SubKey: {#MyAppName}.Project; ValueType: string; ValueData: {#MyAppName} Project File; Flags: uninsdeletekey
Root: HKCR; SubKey: {#MyAppName}.Project\Shell\Open\Command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Flags: uninsdeletevalue


[Run]
Filename: {app}\{#MyAppExeName}; Description: {cm:LaunchProgram,{#MyAppName}}; Flags: nowait postinstall skipifsilent


; Debug output of preprocessor up to current line, at end == complete file
;#expr SaveToFile("preprocessed.iss")
