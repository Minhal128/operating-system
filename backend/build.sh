#!/bin/bash

echo "Building EDR System..."

# Create build directory
mkdir -p build

# Detect platform
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
    PLATFORM="WINDOWS_PLATFORM"
    OUTPUT="build/edr_system.exe"
    LIBS="-lws2_32"
else
    PLATFORM="LINUX_PLATFORM"
    OUTPUT="build/edr_system"
    LIBS="-lpthread -lrt"
fi

echo "Platform: $PLATFORM"
echo "Compiling..."

g++ -std=c++17 -D$PLATFORM -I./src \
    src/main.cpp \
    src/core/file_handler.cpp \
    src/core/process_manager.cpp \
    src/core/memory_manager.cpp \
    src/core/ipc_manager.cpp \
    src/monitoring/monitor.cpp \
    src/monitoring/fft_engine.cpp \
    src/monitoring/anomaly_detector.cpp \
    src/simulation/attack_simulator.cpp \
    src/utils/logger.cpp \
    src/utils/performance.cpp \
    -o $OUTPUT \
    $LIBS

if [ $? -eq 0 ]; then
    echo "Build successful! Executable: $OUTPUT"
    chmod +x $OUTPUT
else
    echo "Build failed!"
    exit 1
fi
