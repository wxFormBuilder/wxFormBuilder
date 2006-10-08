;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; File:        wxFormBuilder.iss
; Author:      Ryan Pusztai & Ryan Mulder
; Date:        02/07/2006
; Copyright:   (c) 2006 Ryan Pusztai <rpusztai@gmail.com>
;              (c) 2006 Ryan Mulder <rjmyst3@gmail.com>
; License:     wxWindows license
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define UNICODE 1

#define MyAppVer "3.0.02"
#define MyAppName "wxFormBuilder"
#define MyAppPublisher "José Antonio Hurtado"
#define MyAppURL "http://wxformbuilder.org"
#define MyAppExeName "wxFormBuilder.exe"
#define wxFormBuilderMinVer "2.0.75"
#define Additions "wxAdditions_setup.exe"

[_ISToolDownload]
Source: http://wxformbuilder.sourceforge.net/beta/wxAdditions27_setup.exe; DestDir: {tmp}; DestName: wxAdditions_setup.exe

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
#if UNICODE
OutputBaseFilename={#MyAppName}_v{#MyAppVer}-beta1
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
InfoAfterFile=files\Changelog.txt
LicenseFile=files\license.txt
#if UNICODE
MinVersion=0,4.0.1381sp6
#endif


[Messages]
BeveledLabel={#MyAppName} v{#MyAppVer} Beta 1

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[Files]
#if UNICODE
Source: files\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs
#else
Source: files9x\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs
#endif
Source: source\*; DestDir: {app}\source; Flags: ignoreversion recursesubdirs createallsubdirs; Components: main\srccode

[Icons]
Name: {group}\{#MyAppName}; Filename: {app}\{#MyAppExeName}
;Name: {group}\{#MyAppName} Help; Filename: {app}\{#MyAppName}.hlp
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}
Name: {userdesktop}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: desktopicon

[Run]
Filename: {app}\{#MyAppExeName}; Description: {cm:LaunchProgram,{#MyAppName}}; Flags: nowait postinstall skipifsilent
Filename: {tmp}\{#additions}; WorkingDir: {tmp}; StatusMsg: Installing wxAdditions ...; Flags: hidewizard; Check: wxAdditionsCheck; Parameters: "/wxfbpath=""{app}"""

[Components]
Name: main; Description: wxFormBuilder (required); Flags: fixed dontinheritcheck checkablealone; Types: custom compact full
Name: main\srccode; Description: SourceCode; Types: custom; Flags: dontinheritcheck checkablealone disablenouninstallwarning

[Registry]
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\{#MyAppExeName}; ValueType: string; ValueData: {app}\{#MyAppExeName}; Flags: uninsdeletekey
Root: HKCR; SubKey: .fbp; ValueType: string; ValueData: {#MyAppName}.Project; Flags: uninsdeletekey
Root: HKCR; SubKey: {#MyAppName}.Project\DefaultIcon; ValueType: string; ValueName: ; ValueData: {app}\{#MyAppExeName}; Flags: uninsdeletekey
Root: HKCR; SubKey: {#MyAppName}.Project; ValueType: string; ValueData: {#MyAppName} Project File; Flags: uninsdeletekey
Root: HKCR; SubKey: {#MyAppName}.Project\Shell\Open\Command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Flags: uninsdeletevalue

[_ISToolPreCompile]
Name: create_source_package.bat; Parameters: ; Flags: runminimized

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
			end;
		end else begin
			result := true;
		end;
	end;
end;
// -- END -- Version checking

// -- Functions for custom page and downloading
var
  plugins_page: TWizardPage;
  { Plug-in global variables. In this section you need to create a TCheckBox
    for each plugin you want to display in the plugins page. }
  // wxAdditions plugin.
  wxAdditionsCheckBox: TCheckBox;
  // Sample plugin.
  sampleCheckBox: TCheckBox;

function CreateCustomOptionPage(AfterId: Integer; Caption, SubCaption, {AImageFileName,}
	wxFBPluginDescCaption, InstructionsCaption, TipCaption,
	Plugin1Caption, Plugin1Hint,
	Plugin2Caption, Plugin2Hint: String;
	var Plugin1, Plugin2: TCheckBox
  ): TWizardPage;
var
  Page: TWizardPage;
  Label1, Label2: TNewStaticText;
  Label3: TLabel;
  Bevel1: TBevel;

begin
  Page := CreateCustomPage(AfterID, Caption, SubCaption);

  { wxFormBuilder Plug-in Description }
  Label1 := TNewStaticText.Create(Page);
  with Label1 do begin
    AutoSize := False;
    Width := Page.SurfaceWidth - Left;
    WordWrap := True;
    Caption := wxFBPluginDescCaption;
    Parent := Page.Surface;
  end;
  WizardForm.AdjustLabelHeight(Label1);

  { User instructions }
  Label2 := TNewStaticText.Create(Page);
  with Label2 do begin
    AutoSize := False;
    //Top := Page.SurfaceHeight - ScaleY(32);
    Top := Label1.Top + Label1.Height + ScaleY(12);
    Width := Page.SurfaceWidth - Left;
    Font.Style := [fsBold];
    Caption := InstructionsCaption;
    WordWrap := True;
    Parent := Page.Surface;
  end;
  WizardForm.AdjustLabelHeight(Label2);

  { Bevel1 }
  Bevel1 := TBevel.Create(Page);
  with Bevel1 do
  begin
    Parent := Page.Surface;
    Top := Label2.Top + Label2.Height + ScaleY(2);
    Width := Page.SurfaceWidth;
    Height := Page.SurfaceHeight - Label1.Height - Label2.Height - ScaleY(28);
  end;

  { Tip text }
  Label3 := TLabel.Create(Page);
  with Label3 do begin
    Top := Bevel1.Top + Bevel1.Height;
    Width := Page.SurfaceWidth;
    Alignment := taRightJustify;
    Font.Color := -16777204;
    Font.Height := ScaleY(-9);
    Caption := TipCaption;
    Parent := Page.Surface;
  end;

  { Plugin1 }
  wxAdditionsCheckBox := TCheckBox.Create(Page);
  with wxAdditionsCheckBox do begin
    TabOrder := 0;
    Parent := Page.Surface;
    Left := ScaleX(8);
    Top := Bevel1.Top + ScaleY(4);
    Width := Page.SurfaceWidth - ScaleX(16);
    Caption := Plugin1Caption;
    Hint := Plugin1Hint;
    ShowHint := True;
    Checked := True;
    State := cbChecked;
  end;

  { Plugin2 }
  sampleCheckBox := TCheckBox.Create(Page);
  with sampleCheckBox do begin
    Parent := Page.Surface;
    Left := ScaleX(8);
    Top := Plugin1.Top + Plugin1.Height + ScaleY(4);
    Width := Page.SurfaceWidth - ScaleX(16);
    Caption := Plugin2Caption;
    Hint := Plugin2Hint;
    ShowHint := True;
  end;

  Result := Page;
end;

procedure CreateCustomPages;
var
  Caption: String;
  SubCaption1: String;
  Description: String;
  Instructions: String;
  Tip: String;

  // Plugins Start Here:
  p1Caption: String;
  p1Hint: String;
  p2Caption: String;
  p2Hint: String;

begin
  Caption := 'wxFormBuilder Plug-ins';
  SubCaption1 := 'Would you like to download and install wxFormBuilder plug-ins?';

  // Plugins descriptions text.
  Description :=
    'wxFormBuilder v3.0+ includes support for plug-ins that extend wxFormBuilders ' +
    'abilities. This makes it easy to add new controls to wxFormBuilder. ' +
    'Please look and determine what plug-ins you would like to install.';

  // User instructions text.
  Instructions := 'Select whether you would like to download and install wxFormBuilder ' +
    'plug-ins, then click Next.';

  // Tip text.
  Tip := 'Tip: Hover mouse cursor over plug-in to get more information';

  { wxAdditions :Plugin1 }
  p1Caption := 'wxAdditions plug-in.  Source and binaries (26.5MB)';
  p1Hint := 'Includes wxScintilla, wxPropGrid, wxFlatNotebook, wxPlotCtrl, and AWX.';

  { Sample2Plugin :Plugin2 }
  p2Caption := 'Sample wxFormBuilder plug-in. Binary only (456KB)';
  p2Hint := 'This doesn''t actually install anything. It is just a test.';

  // Create the actual page.
  plugins_page := CreateCustomOptionPage(wpSelectProgramGroup,
	Caption, SubCaption1,
	{ImageFileName,}
	Description, Instructions, Tip,
	p1Caption, p1Hint,
	p2Caption, p2Hint,
	wxAdditionsCheckBox, sampleCheckBox
  );
end;

function IsCheckBoxChecked( box: TCheckBox ) : Boolean;
begin
  if WizardSilent() then
  begin
    Result := False;
  end else
  begin
	Result := box.Checked;
  end;
end;

{ Plugin install check functions. These need to be implimented for each new plugin. }
function wxAdditionsCheck : Boolean;
begin
  Result := IsCheckBoxChecked( wxAdditionsCheckBox );
end;

function sampleCheck : Boolean;
begin
  Result := IsCheckBoxChecked( sampleCheckBox );
end;

procedure InitializeWizard;
begin
  CreateCustomPages;

  { Add retrieving previous data for each plugin here: }
  wxAdditionsCheckBox.Checked := GetPreviousData('wxAdditions', '1') = '1';
end;

procedure RegisterPreviousData(PreviousDataKey: Integer);
begin
  { Add setting the data for each plugin here: }
  SetPreviousData(PreviousDataKey, 'wxAdditions', IntToStr(Ord(wxAdditionsCheckBox.Checked)));
end;

function NextButtonClick(CurPage: Integer): Boolean;
begin
	if (wxAdditionsCheckBox.Checked) and not (WizardSilent()) then
	begin
		Result := istool_download(CurPage);
	end
	else begin
		Result := True;
	end;
end;
// -- END -- Functions for custom page and downloading
