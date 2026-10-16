$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$buildDirectory = Join-Path $projectRoot '.pio/host-tests'
New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
$compiler = Get-Command g++, clang++, cl -ErrorAction SilentlyContinue | Select-Object -First 1
if (-not $compiler) {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
    if (Test-Path -LiteralPath $vswhere) {
        $installation = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        if ($installation) {
            & (Join-Path $installation 'Common7/Tools/Launch-VsDevShell.ps1') -Arch amd64 -HostArch amd64 -SkipAutomaticLocation
            $compiler = Get-Command cl -ErrorAction SilentlyContinue
        }
    }
}
if (-not $compiler) { throw 'Install a C++17 compiler (GCC, Clang or Visual Studio C++ Build Tools).' }
Push-Location $buildDirectory
try {
    $sources = @(
        (Join-Path $projectRoot 'test/host/main.cpp'),
        (Join-Path $projectRoot 'src/Application.cpp'),
        (Join-Path $projectRoot 'src/ApplicationDisplay.cpp'),
        (Join-Path $projectRoot 'src/Controllers/DefuseGameController.cpp'),
        (Join-Path $projectRoot 'src/Controllers/DominationGameController.cpp')
    )
    $includes = @((Join-Path $projectRoot 'test/host/fakes'), (Join-Path $projectRoot 'src'))
    if ($compiler.Name -eq 'cl.exe' -or $compiler.Name -eq 'cl') {
        & $compiler.Source /nologo /std:c++17 /EHsc /W4 /WX /wd4996 "/I$($includes[0])" "/I$($includes[1])" $sources /Fe:regression.exe
    } else {
        & $compiler.Source -std=c++17 -Wall -Wextra -Werror "-I$($includes[0])" "-I$($includes[1])" $sources -o regression.exe
    }
    if ($LASTEXITCODE -ne 0) { throw 'Regression test compilation failed.' }
    & ./regression.exe
    if ($LASTEXITCODE -ne 0) { throw 'Regression tests failed.' }
} finally {
    Pop-Location
}
