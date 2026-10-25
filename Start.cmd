@echo off
chcp 65001 >nul
cd /d "%~dp0"
if not exist "bin\c-task-lab.exe" (
  echo Please build first: powershell -ExecutionPolicy Bypass -File .\build.ps1
  pause
  exit /b 1
)
"bin\c-task-lab.exe"
pause
