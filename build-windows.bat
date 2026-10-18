@echo off
setlocal
cd /d "%~dp0"
where cmake >nul 2>nul
if errorlevel 1 (
  echo CMake is not installed or not in PATH.
  exit /b 1
)
echo Configuring IRONVEIN...
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
if errorlevel 1 exit /b 1
echo Building VST3 and standalone...
cmake --build build --config Release --target Ironvein_VST3 Ironvein_Standalone --parallel 2
if errorlevel 1 exit /b 1
echo.
echo Output: build\Ironvein_artefacts\Release\
endlocal
