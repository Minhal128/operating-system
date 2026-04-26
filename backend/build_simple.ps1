# Simple PowerShell build script for EDR System

Write-Host "Building EDR System..." -ForegroundColor Cyan

# Create build directory
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Try to compile
Write-Host "Compiling with g++..." -ForegroundColor Yellow

$sources = @(
    "src/main.cpp",
    "src/core/file_handler.cpp",
    "src/core/process_manager.cpp",
    "src/core/memory_manager.cpp",
    "src/core/ipc_manager.cpp",
    "src/monitoring/monitor.cpp",
    "src/monitoring/fft_engine.cpp",
    "src/monitoring/anomaly_detector.cpp",
    "src/simulation/attack_simulator.cpp",
    "src/utils/logger.cpp",
    "src/utils/performance.cpp"
)

    $cmd = "g++ -std=c++17 -DWINDOWS_PLATFORM -I./src " + ($sources -join " ") + " -o build/edr_backend.exe -lws2_32"

    Write-Host "Command: $cmd" -ForegroundColor Gray

    try {
        # Build Unified Backend
        Write-Host "`nCompiling Unified Backend..." -ForegroundColor Yellow
        Invoke-Expression $cmd

        if ($LASTEXITCODE -eq 0) {
            Write-Host "Build of edr_backend successful!" -ForegroundColor Green
            Write-Host "`nBuild complete! Running this executable will start the automated demo (all modes)." -ForegroundColor Green
            Write-Host "Executable: build/edr_backend.exe" -ForegroundColor Green
        } else {
            Write-Host "`nBuild failed!" -ForegroundColor Red
        }
} catch {
    Write-Host "`nError: $_" -ForegroundColor Red
    Write-Host "`nMake sure g++ is installed and in your PATH" -ForegroundColor Yellow
    Write-Host "Install MinGW-w64 or MSYS2 to get g++" -ForegroundColor Yellow
}
