# Builds the Windows installer of MovieMetaM in one go:
#   1. Release build (build\windows-release)   2. windeployqt (build\windows-deploy)
#   3. Inno Setup  ->  build\MovieMetaM-windows-Setup.exe
# Run: tools\build-windows.cmd (double click) or powershell -ExecutionPolicy Bypass -File tools\build-windows.ps1
# Needs: Visual Studio 2022 Build Tools (MSVC), Qt 6 for MSVC, Inno Setup 6, tmdb_token.txt.
param(
    [string]$QtDir = 'C:\Qt\6.12.0\msvc2022_64',
    [string]$QtTools = 'C:\Qt\Tools'
)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$buildDir = Join-Path $root 'build\windows-release'
$deployDir = Join-Path $root 'build\windows-deploy'

# Version from CMakeLists.txt (set(PROJECT_VERSION "0.9.2"))
$version = (Select-String -Path (Join-Path $root 'CMakeLists.txt') -Pattern 'set\(PROJECT_VERSION "([^"]+)"\)').Matches[0].Groups[1].Value
Write-Host "MovieMetaM $version" -ForegroundColor Cyan

# Without the token the installed app could not search TMDB
$tokenFile = Join-Path $root 'tmdb_token.txt'
if (-not (Test-Path $tokenFile) -or -not (Get-Content $tokenFile -Raw).Trim()) {
    throw 'tmdb_token.txt is missing or empty: the TMDB search would not work'
}

# Tools
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vsPath = if (Test-Path $vswhere) { & $vswhere -latest -products * -property installationPath } else { $null }
if (-not $vsPath) { $vsPath = Get-ChildItem "${env:ProgramFiles(x86)}\Microsoft Visual Studio\*\BuildTools" -Directory | Select-Object -Last 1 -ExpandProperty FullName }
$vcvars = Join-Path $vsPath 'VC\Auxiliary\Build\vcvars64.bat'
$cmake = Join-Path $QtTools 'CMake_64\bin\cmake.exe'
$ninja = Join-Path $QtTools 'Ninja\ninja.exe'
$windeployqt = Join-Path $QtDir 'bin\windeployqt.exe'
$iscc = @("$env:LOCALAPPDATA\Programs\Inno Setup 6\ISCC.exe", "${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe") |
        Where-Object { Test-Path $_ } | Select-Object -First 1
$redist = Get-ChildItem (Join-Path $vsPath 'VC\Redist\MSVC') -Recurse -Filter 'vc_redist.x64.exe' -ErrorAction SilentlyContinue |
          Select-Object -First 1 -ExpandProperty FullName
foreach ($tool in @($vcvars, $cmake, $ninja, $windeployqt, $iscc)) {
    if (-not $tool -or -not (Test-Path $tool)) { throw "Missing: $tool" }
}

# 1. Release build
Write-Host '1. Release build' -ForegroundColor Cyan
cmd /c "call `"$vcvars`" >nul && `"$cmake`" -S `"$root`" -B `"$buildDir`" -G Ninja -DCMAKE_MAKE_PROGRAM=`"$ninja`" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=`"$QtDir`" && `"$cmake`" --build `"$buildDir`""
if ($LASTEXITCODE -ne 0) { throw 'Build failed' }

# 2. Collect Qt and plugins next to the exe (the VC runtime comes with the installer; Qt's own
#    texts are embedded in the exe, :/i18n/qtbase_*.qm)
Write-Host '2. windeployqt' -ForegroundColor Cyan
if (Test-Path $deployDir) { Remove-Item $deployDir -Recurse -Force }
New-Item -ItemType Directory $deployDir | Out-Null
Copy-Item (Join-Path $buildDir 'MovieMetaM.exe') $deployDir
& $windeployqt --release --no-compiler-runtime --no-system-d3d-compiler --no-opengl-sw --no-translations (Join-Path $deployDir 'MovieMetaM.exe')
if ($LASTEXITCODE -ne 0) { throw 'windeployqt failed' }

# 3. Installer
Write-Host '3. Inno Setup' -ForegroundColor Cyan
$defines = @("/DAppVersion=$version", "/DSourceDir=$deployDir")
if ($redist) { $defines += "/DRedist=$redist" } else { Write-Warning 'vc_redist.x64.exe not found: installer without VC runtime' }
& $iscc @defines (Join-Path $PSScriptRoot 'MovieMetaM.iss')
if ($LASTEXITCODE -ne 0) { throw 'Inno Setup failed' }
Write-Host "Done: build\MovieMetaM-windows-Setup.exe ($version)" -ForegroundColor Green
