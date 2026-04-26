@echo off
REM Build script for EDR System on Windows

echo Building EDR System for Windows...

REM Create build directory
if not exist build mkdir build
cd build

REM Run CMake
cmake .. -G "Visual Studio 16 2019"
if %errorlevel% neq 0 (
    echo CMake configuration failed!
    exit /b %errorlevel%
)

REM Build
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo Build failed!
    exit /b %errorlevel%
)

echo Build complete! Executable: build\Release\edr_system.exe
cd ..
