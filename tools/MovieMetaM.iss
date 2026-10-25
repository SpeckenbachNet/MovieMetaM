; Installer for MovieMetaM (Inno Setup 6). Built by build-windows.ps1, which passes
;   /DAppVersion=<version>  /DSourceDir=<windeployqt folder>  /DRedist=<vc_redist.x64.exe>
; Result: build\MovieMetaM-windows-Setup.exe (fixed name for the release)

#ifndef AppVersion
  #define AppVersion "0.0.0"
#endif
#ifndef SourceDir
  #define SourceDir "..\build\windows-deploy"
#endif

#define AppName "MovieMetaM"
#define AppPublisher "SpeckenbachNet"
#define AppExe "MovieMetaM.exe"

[Setup]
; Fixed ID: a newer version replaces the installed one
AppId={{60123BD2-9011-47C4-B6E8-8F3F31F5B215}
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName} {#AppVersion}
AppPublisher={#AppPublisher}
AppPublisherURL=https://software.speckenbach.net/moviemetam
VersionInfoVersion={#AppVersion}
DefaultDirName={autopf}\{#AppName}
DefaultGroupName={#AppName}
DisableProgramGroupPage=yes
PrivilegesRequired=admin
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0
SetupIconFile=..\ressources\icons\app_icon.ico
; GPL v3, shown as a page of the installer
LicenseFile=..\LICENSE
UninstallDisplayIcon={app}\{#AppExe}
OutputDir=..\build
OutputBaseFilename={#AppName}-windows-Setup
Compression=lzma2/max
SolidCompression=yes
WizardStyle=modern
; The app must be closed to be replaced (it asks about unsaved changes itself)
CloseApplications=yes
; "Open with" entries for .mp4 / .m4v
ChangesAssociations=yes
; The language of the system, the dialog only when none of the list fits
ShowLanguageDialog=auto

[Languages]
; English first: the default for systems whose language is not in the list
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"
Name: "fr"; MessagesFile: "compiler:Languages\French.isl"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "it"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "pt_BR"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "ja"; MessagesFile: "compiler:Languages\Japanese.isl"
Name: "ko"; MessagesFile: "compiler:Languages\Korean.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#SourceDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\LICENSE"; DestDir: "{app}"; DestName: "LICENSE.txt"; Flags: ignoreversion
Source: "..\LGPL-3.0.txt"; DestDir: "{app}"; Flags: ignoreversion
#ifdef Redist
Source: "{#Redist}"; DestDir: "{tmp}"; DestName: "vc_redist.x64.exe"; Flags: deleteafterinstall; Check: NeedsVCRedist
#endif

[Icons]
Name: "{autoprograms}\{#AppName}"; Filename: "{app}\{#AppExe}"
Name: "{autodesktop}\{#AppName}"; Filename: "{app}\{#AppExe}"; Tasks: desktopicon

[Registry]
; "Open with" for .mp4 and .m4v – not the default program, the video player stays the default
Root: HKA; Subkey: "Software\Classes\MovieMetaM.Movie"; ValueType: string; ValueName: ""; ValueData: "MP4 Video"; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\MovieMetaM.Movie\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: """{app}\{#AppExe}"",0"
Root: HKA; Subkey: "Software\Classes\MovieMetaM.Movie\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#AppExe}"" ""%1"""
Root: HKA; Subkey: "Software\Classes\.mp4\OpenWithProgids"; ValueType: string; ValueName: "MovieMetaM.Movie"; ValueData: ""; Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\.m4v\OpenWithProgids"; ValueType: string; ValueName: "MovieMetaM.Movie"; ValueData: ""; Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\Applications\{#AppExe}"; ValueType: string; ValueName: "FriendlyAppName"; ValueData: "{#AppName}"; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\Applications\{#AppExe}\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#AppExe}"" ""%1"""
Root: HKA; Subkey: "Software\Classes\Applications\{#AppExe}\SupportedTypes"; ValueType: string; ValueName: ".mp4"; ValueData: ""
Root: HKA; Subkey: "Software\Classes\Applications\{#AppExe}\SupportedTypes"; ValueType: string; ValueName: ".m4v"; ValueData: ""

[Run]
#ifdef Redist
; Visual C++ runtime (MSVC 2022), only if missing
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "Visual C++ Runtime ..."; Check: NeedsVCRedist; Flags: waituntilterminated
#endif
Filename: "{app}\{#AppExe}"; Description: "{cm:LaunchProgram,{#AppName}}"; Flags: nowait postinstall skipifsilent

[Code]
// The runtime of Visual Studio 2015-2022 (and newer) registers itself here; the app needs 14.40 or newer
function NeedsVCRedist: Boolean;
var
  Installed, Major, Minor: Cardinal;
begin
  Result := True;
  if RegQueryDWordValue(HKLM64, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Installed', Installed)
     and (Installed = 1)
     and RegQueryDWordValue(HKLM64, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Major', Major)
     and RegQueryDWordValue(HKLM64, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Minor', Minor) then
    Result := (Major < 14) or ((Major = 14) and (Minor < 40));
end;
