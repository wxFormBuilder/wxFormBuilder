;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; File:        wxFormBuilder.iss
; Author:      Ryan Pusztai & Ryan Mulder
; Date:        02/07/2006
; Copyright:   (c) 2007 Ryan Pusztai <rpusztai@gmail.com>
;              (c) 2007 Ryan Mulder <rjmyst3@gmail.com>
; License:     wxWindows license
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define UNICODE 1

#define MyAppVer "3.2.2"
#define MyAppName "wxFormBuilder"
#define MyAppPublisher "Jose Antonio Hurtado"
#define MyAppURL "http://wxformbuilder.org"
#define MyAppExeName "wxFormBuilder.exe"
#define wxFormBuilderMinVer "3.0.57"

[Setup]
AppName={#MyAppName}
AppVerName={#MyAppName} {#MyAppVer}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DisableDirPage=false
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=false
#if defined UNICODE
OutputBaseFilename={#MyAppName}_v{#MyAppVer}-beta
#else
OutputBaseFilename={#MyAppName}_v{#MyAppVer}-9xME
#endif
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
#if defined UNICODE
;MinVersion=0,4.0.1381sp6
MinVersion=0,5.0
#endif

; -- Pre-Build Step
#expr Exec( "create_source_package.bat", NULL, NULL, 1, SW_SHOWMINIMIZED )

[Messages]
BeveledLabel={#MyAppName} v{#MyAppVer}

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[Files]
#if defined UNICODE
Source: ..\..\output\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs; Excludes: .svn\*, *d.exe, *d.dll, wxmsw28ud_*, wxmsw28umd_*, Thumbs.db
#else
Source: files9x\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs; Excludes: .svn\*, *d.exe, *d.dll, wxmsw28ud_*, wxmsw28umd_*, Thumbs.db
#endif
Source: source\*; DestDir: {app}\source; Flags: ignoreversion recursesubdirs createallsubdirs; Components: main\srccode

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
Name: main\srccode; Description: SourceCode; Types: custom; Flags: dontinheritcheck checkablealone disablenouninstallwarning

[Registry]
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\{#MyAppExeName}; ValueType: string; ValueData: {app}\{#MyAppExeName}; Flags: uninsdeletekey
Root: HKCR; SubKey: .fbp; ValueType: string; ValueData: {#MyAppName}.Project; Flags: uninsdeletekey
Root: HKCR; SubKey: {#MyAppName}.Project\DefaultIcon; ValueType: string; ValueName: ; ValueData: {app}\{#MyAppExeName}; Flags: uninsdeletekey
Root: HKCR; SubKey: {#MyAppName}.Project; ValueType: string; ValueData: {#MyAppName} Project File; Flags: uninsdeletekey
Root: HKCR; SubKey: {#MyAppName}.Project\Shell\Open\Command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Flags: uninsdeletevalue
Root: HKCU; SubKey: Software\wxformbuilder\mainframe\editor; ValueType: none; ValueName: notebook_style; Flags: deletevalue; Check: ShouldResetLayout
Root: HKCU; SubKey: Software\wxformbuilder\mainframe\editor\cpp; ValueType: none; ValueName: notebook_style; Flags: deletevalue; Check: ShouldResetLayout
Root: HKCU; SubKey: Software\wxformbuilder\mainframe\objectInspector; ValueType: none; ValueName: notebook_style; Flags: deletevalue; Check: ShouldResetLayout
Root: HKCU; SubKey: Software\wxformbuilder\palette; ValueType: none; ValueName: notebook_style; Flags: deletevalue; Check: ShouldResetLayout

[Code]
// -- Version checking functions
function GetPathInstalled( AppID: String ): String;
var
   sPrevPath: String;
begin
  // Debug Stuff
  //MsgBox( AppID + ' was passed into GetPathInstalled', mbInformation, MB_OK);

  sPrevPath := '';
  if not RegQueryStringValue( HKLM,
    'Software\Microsoft\Windows\CurrentVersion\Uninstall\'+AppID+'_is1',
      'Inno Setup: App Path', sPrevpath) then
    RegQueryStringValue( HKCU, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\'+AppID+'_is1' ,
      'Inno Setup: App Path', sPrevpath);

  // Debug Stuff
  //MsgBox( 'Installed Path: ' + sPrevPath, mbInformation, MB_OK);

  Result := sPrevPath;
end;

function GetInstalledVersion( AppID: String ): String;
var
   sPrevPath: String;

begin
  sPrevPath := '';
  if not RegQueryStringValue( HKLM,
    'Software\Microsoft\Windows\CurrentVersion\Uninstall\'+AppID+'_is1',
		'DisplayVersion', sPrevpath) then
    RegQueryStringValue( HKCU, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\'+AppID+'_is1' ,
		'DisplayVersion', sPrevpath);

  Result := sPrevPath;
end;

function GetPathUninstallString( AppID: String ): String;
var
   sPrevPath: String;
begin
  sPrevPath := '';
  if not RegQueryStringValue( HKLM,
    'Software\Microsoft\Windows\CurrentVersion\Uninstall\'+AppID+'_is1',
		'UninstallString', sPrevpath) then
    RegQueryStringValue( HKCU, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\'+AppID+'_is1' ,
		'UninstallString', sPrevpath);

  Result := sPrevPath;
end;

function InitializeSetup(): boolean;
var
	ResultCode: Integer;
	wxFormBuilderVersion: String;
	sUninstallEXE: String;

begin
	wxFormBuilderVersion:= GetInstalledVersion('{#MyAppName}');
	sUninstallEXE:= RemoveQuotes(GetPathUninstallString('{#MyAppName}'));

	// Debug Stuff
	//MsgBox('wxAdditions Version ' + wxAdditionsVersion + ' was found' #13 'The length is ' + IntToStr(Length(wxAdditionsVersion)), mbInformation, MB_OK);
	//MsgBox('Version ' + wxVersion + ' was found' #13 'The length is ' + IntToStr(Length(wxVersion)), mbInformation, MB_OK);
	//MsgBox('Uninstall is located at : ' + sUninstallEXE, mbInformation, MB_OK);

	// Check to make sure there is an exceptable version of wxAdditions installed.
	if Length(wxFormBuilderVersion) = 0 then begin
		result:= true;
	end else begin
		//MsgBox('wxFormBuilder minimum version: ' + '{#wxFormBuilderMinVer}' #13 'wxFormBuilder current version: ' + wxFormBuilderVersion, mbInformation, MB_OK);
		if CompareText( wxFormBuilderVersion, '{#wxFormBuilderMinVer}' ) <= 0 then begin
			if FileExists(sUninstallEXE) then begin
				if WizardSilent() then begin
					// Just uninstall without asking because we are in silent mode.
					Exec(sUninstallEXE,	'/SILENT', GetPathInstalled('{#MyAppName}'),
							SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode);

					// Make sure that Setup is visible and the foreground window
					BringToFrontAndRestore;
					result := true;
				end else begin
					// Ask if they really want to uninstall because we are in the default installer.
					if MsgBox('Version ' + wxFormBuilderVersion + ' of {#MyAppName} was detected.' #13 'It is recommended that you uninstall the old version first before continuing.' + #13 + #13 + 'Would you like to uninstall it now?', mbInformation, MB_YESNO) = IDYES then begin
						Exec(sUninstallEXE,	'/SILENT', GetPathInstalled('{#MyAppName}'),
							SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode);

						// Make sure that Setup is visible and the foreground window
						BringToFrontAndRestore;
						result := true;
					end else begin
						result := true;
					end;
				end;
			end else begin
				result := true;
			end;
		end else begin
			result := true;
		end;
	end;
end;

function ShouldResetLayout(): boolean;
var
	wxFormBuilderVersion: String;
begin
	wxFormBuilderVersion:= GetInstalledVersion('{#MyAppName}');

	if CompareText( wxFormBuilderVersion, '3.0.44' ) <= 0 then begin
		result := true;
	end else begin
		result := false;
	end;
end;
// -- END -- Version checking







