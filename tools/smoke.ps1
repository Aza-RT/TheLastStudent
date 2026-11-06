param([string]$Godot = '', [string]$Config = 'Release', [switch]$Visual)
$ErrorActionPreference = 'Stop'
$taskRoot = Split-Path -Parent $PSScriptRoot
if (-not $Godot) {
    if ($env:GODOT_BIN) { $Godot = $env:GODOT_BIN }
    else { $Godot = "$taskRoot/.tools/Godot_v4.7.2-stable_win64_console.exe" }
}
Push-Location $taskRoot
try {
    New-Item -ItemType Directory -Force artifacts | Out-Null
    ctest --test-dir build -C $Config --output-on-failure
    if ($LASTEXITCODE -ne 0) { throw 'Native tests failed' }
    $taskImport = & $Godot --headless --path game --editor --import --quit 2>&1 | Out-String
    $taskImport | Write-Output
    if ($LASTEXITCODE -ne 0 -or $taskImport -match 'SCRIPT ERROR:|ERROR:') { throw 'Import/script check failed' }
    $taskSmoke = & $Godot --headless --path game --script res://scripts/smoke_test.gd 2>&1 | Out-String
    $taskSmoke | Write-Output
    if ($LASTEXITCODE -ne 0 -or $taskSmoke -match 'SCRIPT ERROR:|ERROR:|SMOKE FAIL' -or $taskSmoke -notmatch 'PUZZLE_SMOKE PASS') { throw 'Puzzle smoke failed' }
    if ($Visual) {
        & ".\build\$Config\gl_testbench.exe" --smoke
        if ($LASTEXITCODE -ne 0) { throw 'OpenGL testbench smoke failed' }
        & $Godot --path game --disable-vsync -- --capture
        if ($LASTEXITCODE -ne 0) { throw 'Godot capture failed' }
    }
} finally { Pop-Location }
