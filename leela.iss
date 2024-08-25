#define MyAppName "Leela"
#define MyAppMainExe "leela.exe"
#define MyAppVersion GetVersionNumbersString("x64\Release\" + MyAppMainExe)
;#define MyAppVersion GetVersionNumbersString("build\exe.win-amd64-3.11\" + MyAppMainExe)

[Setup]
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher=Leela Development Team
WizardStyle=modern
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName=Leela
UninstallDisplayIcon={app}\{#MyAppMainExe}
UninstallDisplayName={#MyAppName}
Compression=lzma2
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64
OutputBaseFilename=LeelaSetup-{#MyAppVersion}
; default OutputDir is 'Output' in the same directory containing his script

SetupIconFile=leela.ico
;WizardImageFile=starkey_logo_small.bmp
;WizardImageStretch=False

DisableDirPage=no

[Files]
Source: "x64\Release\leela.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "external\FreeType\release_dll\win64\freetype.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "external\glew-2.1.0\bin\Release\x64\glew32.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\prj\leela\external\SDL2-2.30.1\lib\x64\SDL2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "leela\shaders\*"; DestDir: "{app}\shaders"; Flags: ignoreversion recursesubdirs;
Source: "leela\textures\*"; DestDir: "{app}\textures"; Flags: ignoreversion recursesubdirs;
Source: "external\fonts\*.ttf"; DestDir: "{app}\fonts"; Flags: ignoreversion;
Source: "leela.ico"; DestDir: "{app}"; Flags: ignoreversion;


[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppMainExe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppMainExe}"; IconFilename: "{app}\leela.ico"; Tasks: desktopicon

[Run]
Filename: {app}\{#MyAppName}.exe; Description: Run application after install; Flags: postinstall nowait

[Code]
// remove previous 64-bits installation
function Uninstall64bitsVersion () : Boolean;
var
  sUninstall: string;
  ResultCode: Integer;
begin
  Result := True;
  // Try getting the uninstall string.
  if RegQueryStringValue (HKLM,
      'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppName}_is1',
      'UninstallString', sUninstall) then
  begin
    // uninstall it
    ShellExec ('', sUninstall, '/SILENT', '', SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode);
  end;
end;

// Called with CurStep=ssInstall just before the actual installation starts, with CurStep=ssPostInstall
// just after the actual installation finishes, and with CurStep=ssDone just before Setup terminates after
// a successful install.
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if (CurStep = ssInstall) then
  begin
    Uninstall64bitsVersion ();
  end;
end;

