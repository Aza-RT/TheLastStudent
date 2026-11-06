param([string]$Godot = '', [switch]$Fallback, [switch]$Editor)
$ErrorActionPreference = 'Stop'
$taskRoot = Split-Path -Parent $PSScriptRoot
if (-not $Godot) {
    if ($env:GODOT_BIN) { $Godot = $env:GODOT_BIN }
    elseif (Test-Path -LiteralPath "$taskRoot/.tools/Godot_v4.7.2-stable_win64_console.exe") { $Godot = "$taskRoot/.tools/Godot_v4.7.2-stable_win64_console.exe" }
    else { $Godot = (Get-Command godot -ErrorAction Stop).Source }
}
$taskVersion = (& $Godot --version | Out-String).Trim()
if (-not $taskVersion.StartsWith('4.7.2.')) { throw "Requires Godot 4.7.2; found $taskVersion" }
if (-not (Test-Path -LiteralPath "$taskRoot/game/bin/tls_extension.dll")) { throw 'Build the native extension first: see BUILD.md' }
& $Godot --headless --path "$taskRoot/game" --editor --import --quit
if ($LASTEXITCODE -ne 0) { throw 'Godot import failed' }
$taskArgs = @('--path', "$taskRoot/game", '--rendering-method', 'gl_compatibility')
if ($Editor) { $taskArgs += '--editor' }
if ($Fallback) { $taskArgs += @('--', '--fallback') }
& $Godot @taskArgs
exit $LASTEXITCODE
