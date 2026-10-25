# 固定版本，校验官方 SHA-256；仅写入项目的 tools/，不改 PATH 或系统设置。
$ErrorActionPreference = 'Stop'
$toolsDir = Join-Path $PSScriptRoot 'tools'
$zigPath = Join-Path $toolsDir 'zig-x86_64-windows-0.15.2\zig.exe'
if (Test-Path -LiteralPath $zigPath) { Write-Host "Compiler already exists: $zigPath"; exit 0 }
New-Item -ItemType Directory -Force -Path $toolsDir | Out-Null
$archivePath = Join-Path $toolsDir 'zig-0.15.2.zip'
$url = 'https://ziglang.org/download/0.15.2/zig-x86_64-windows-0.15.2.zip'
$expectedHash = '3a0ed1e8799a2f8ce2a6e6290a9ff22e6906f8227865911fb7ddedc3cc14cb0c'
Write-Host 'Downloading official Zig 0.15.2 (about 89 MiB); zig cc compiles C.'
if (Get-Command curl.exe -ErrorAction SilentlyContinue) {
    & curl.exe -L --fail --max-time 300 --silent --show-error $url -o $archivePath
    if ($LASTEXITCODE -ne 0) { throw 'Download failed. See the manual compiler links in README.md.' }
} else {
    $ProgressPreference = 'SilentlyContinue'
    Invoke-WebRequest -Uri $url -OutFile $archivePath -TimeoutSec 300
}
if ((Get-FileHash -LiteralPath $archivePath -Algorithm SHA256).Hash -ne $expectedHash) {
    throw 'SHA-256 mismatch. Do not run the downloaded archive.'
}
if (Get-Command tar.exe -ErrorAction SilentlyContinue) {
    & tar.exe -xf $archivePath -C $toolsDir
    if ($LASTEXITCODE -ne 0) { throw 'Archive extraction failed.' }
} else {
    Expand-Archive -LiteralPath $archivePath -DestinationPath $toolsDir -Force
}
& $zigPath version
if ($LASTEXITCODE -ne 0) { throw 'Compiler setup failed.' }
Write-Host 'Ready. Run: .\build.ps1 -Action lesson -Lesson 01_basics'
