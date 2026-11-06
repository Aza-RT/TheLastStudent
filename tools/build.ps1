param([string]$Generator = 'Visual Studio 17 2022', [string]$Config = 'Release')
$ErrorActionPreference = 'Stop'
$taskRoot = Split-Path -Parent $PSScriptRoot
Push-Location $taskRoot
try {
    cmake -S . -B build -G $Generator -A x64 -DTLS_BUILD_EXTENSION=ON
    if ($LASTEXITCODE -ne 0) { throw 'CMake configure failed' }
    cmake --build build --config $Config --parallel 6
    if ($LASTEXITCODE -ne 0) { throw 'C++ build failed' }
    ctest --test-dir build -C $Config --output-on-failure
    if ($LASTEXITCODE -ne 0) { throw 'Native tests failed' }
} finally { Pop-Location }
