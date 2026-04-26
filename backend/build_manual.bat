@echo off
echo Building EDR System manually...

REM Try to find g++ in common locations
set GCC_PATHS=C:\MinGW\bin;C:\msys64\mingw64\bin;C:\Program Files\mingw-w64\bin;C:\TDM-GCC-64\bin

for %%p in (%GCC_PATHS%) do (
    if exist "%%p\g++.exe" (
        set PATH=%%p;%PATH%
        echo Found g++ at %%p
        goto :compile
    )
)

echo ERROR: g++ not found in common locations
echo Please add g++ to your PATH or install MinGW/MSYS2
exit /b 1

:compile
echo Compiling source files...

g++ -std=c++17 -DWINDOWS_PLATFORM -I./src ^
    src/main.cpp ^
    src/core/file_handler.cpp ^
    src/core/process_manager.cpp ^
    src/core/memory_manager.cpp ^
    src/core/ipc_manager.cpp ^
    src/monitoring/monitor.cpp ^
    src/monitoring/fft_engine.cpp ^
    src/monitoring/anomaly_detector.cpp ^
    src/simulation/attack_simulator.cpp ^
    src/utils/logger.cpp ^
    src/utils/performance.cpp ^
    -o build/edr_system.exe ^
    -lws2_32

if %errorlevel% equ 0 (
    echo Build successful! Executable: build\edr_system.exe
) else (
    echo Build failed!
    exit /b 1
)
