param(
    [ValidateSet('build','run','demo','test','lesson','stage','exercise','solution','check')]
    [string]$Action = 'build',
    [string]$Lesson = '01_basics',
    [string]$Stage = '01_task_card',
    [string]$Exercise = '01_temperature',
    [string]$Compiler = ''
)
$ErrorActionPreference = 'Stop'
$projectRoot = $PSScriptRoot
$buildDir = Join-Path $projectRoot 'build'
New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
$prefix = @()
if ($Compiler) {
    $cc = (Get-Command $Compiler -ErrorAction Stop).Source
} else {
    $cc = $null
    foreach ($candidate in @('clang','gcc','zig')) {
        $command = Get-Command $candidate -ErrorAction SilentlyContinue
        if ($command) { $cc = $command.Source; break }
    }
    if (-not $cc) {
        $localZig = Join-Path $projectRoot 'tools\zig-x86_64-windows-0.15.2\zig.exe'
        if (Test-Path -LiteralPath $localZig) { $cc = $localZig }
    }
    if (-not $cc) {
        $hintFile = Join-Path $projectRoot 'compiler.local.txt'
        if (Test-Path -LiteralPath $hintFile) {
            $hint = (Get-Content -LiteralPath $hintFile -Raw).Trim()
            if ($hint -and (Test-Path -LiteralPath $hint -PathType Leaf)) { $cc = $hint }
        }
    }
    if (-not $cc) {
        throw 'No C compiler. Run: powershell -ExecutionPolicy Bypass -File .\setup-compiler.ps1 ; then run build.ps1 again. See README.md.'
    }
}
if ([IO.Path]::GetFileNameWithoutExtension($cc) -eq 'zig') { $prefix = @('cc') }
$flags = @('-std=c17','-Wall','-Wextra','-Wpedantic','-Wconversion','-Wshadow','-g','-I', (Join-Path $projectRoot 'include'))
function Compile-C([string]$Name, [string[]]$Sources) {
    $destination = Join-Path $buildDir ($Name + '.exe')
    $sourcePaths = @($Sources | ForEach-Object { Join-Path $projectRoot $_ })
    & $cc @prefix @flags @sourcePaths '-o' $destination
    if ($LASTEXITCODE -ne 0) { throw "Compilation failed: $Name" }
    return $destination
}
function Run-C([string]$Program, [string[]]$Arguments = @()) {
    & $Program @Arguments
    if ($LASTEXITCODE -ne 0) { throw "Program returned $LASTEXITCODE : $Program" }
}
function Safe-Source([string]$Folder, [string]$Name) {
    if ($Name -notmatch '^[0-9]{2}_[a-z_]+$') { throw 'Use a lesson/exercise/stage basename, e.g. 01_basics.' }
    $relative = "$Folder/$Name.c"
    if (-not (Test-Path -LiteralPath (Join-Path $projectRoot $relative))) { throw "File does not exist: $relative" }
    return $relative
}
$core = @('src/task.c','src/storage.c')
Push-Location $projectRoot
try {
    switch ($Action) {
        'build' { $exe = Compile-C 'c-task-lab' (@('src/main.c','src/input.c') + $core); Write-Host "Built: $exe" }
        'run' { $exe = Compile-C 'c-task-lab' (@('src/main.c','src/input.c') + $core); Run-C $exe }
        'demo' { $exe = Compile-C 'c-task-lab' (@('src/main.c','src/input.c') + $core); Run-C $exe @('--demo') }
        'test' {
            Run-C (Compile-C 'test_core' (@('tests/test_core.c') + $core))
            Run-C (Compile-C 'test_input' @('tests/test_input.c','src/input.c'))
        }
        'lesson' { Run-C (Compile-C "lesson_$Lesson" @(Safe-Source 'lessons' $Lesson)) }
        'stage' { Run-C (Compile-C "stage_$Stage" @(Safe-Source 'stages' $Stage)) }
        'exercise' { Run-C (Compile-C "exercise_$Exercise" @(Safe-Source 'exercises' $Exercise)) }
        'solution' { Run-C (Compile-C "solution_$Exercise" @(Safe-Source 'exercises/solutions' $Exercise)) }
        'check' {
            Run-C (Compile-C 'test_core' (@('tests/test_core.c') + $core))
            Run-C (Compile-C 'test_input' @('tests/test_input.c','src/input.c'))
            Run-C (Compile-C 'c-task-lab' (@('src/main.c','src/input.c') + $core)) @('--demo')
            foreach ($folder in @('lessons','exercises/solutions','stages')) {
                foreach ($file in Get-ChildItem -LiteralPath (Join-Path $projectRoot $folder) -Filter '*.c' | Sort-Object Name) {
                    $exe = Compile-C ($folder.Replace('/','_') + '_' + $file.BaseName) @("$folder/$($file.Name)")
                    if ($file.BaseName -eq '03_menu') { '0' | & $exe; if ($LASTEXITCODE -ne 0) { throw 'Menu stage failed' } }
                    else { Run-C $exe }
                }
            }
            Write-Host 'CHECK PASSED: core, input, demo, lessons, stages, reference solutions.'
            Write-Host 'TODO exercises are excluded: their initial FAIL is expected.'
        }
    }
} finally { Pop-Location }
