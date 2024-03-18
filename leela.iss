#define MyAppName "Leela"
#define MyAppMainExe "leela.exe"
#define MyAppVersion "1.0"
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
Source: "Release\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs; Excludes: "*.pdb,*.ini"
Source: "leela\shaders\*"; DestDir: "{app}\shaders"; Flags: ignoreversion recursesubdirs;
Source: "leela\textures\*"; DestDir: "{app}\textures"; Flags: ignoreversion recursesubdirs;
Source: "external\fonts\*.ttf"; DestDir: "{app}\fonts"; Flags: ignoreversion;


[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppMainExe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppMainExe}"; IconFilename: "{app}\leela.ico"; Tasks: desktopicon


;[Code]
;procedure InitializeWizard();
;begin
;  { Welcome page }
;  { Hide the labels }
;  WizardForm.WelcomeLabel1.Visible := False;
;  WizardForm.WelcomeLabel2.Visible := False;
;  { Stretch image over whole page }
;  WizardForm.WizardBitmapImage.Width :=
;    WizardForm.WizardBitmapImage.Parent.Width;
;
;  { Finished page }
;  { Hide the labels }
;  WizardForm.FinishedLabel.Visible := False;
;  WizardForm.FinishedHeadingLabel.Visible := False;
;  { Stretch image over whole page }
;  WizardForm.WizardBitmapImage2.Width :=
;   WizardForm.WizardBitmapImage2.Parent.Width;
;end;