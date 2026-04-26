# Real-Time Intrusion Detection and Response System

A comprehensive cross-platform (Linux & Windows) endpoint detection and response (EDR) system that monitors system-level activities, detects anomalies using Fast Fourier Transform (FFT), and visualizes operations in real-time.

## 🎯 Features

### Core System Monitoring
- **File I/O Operations**: Cross-platform file handling with performance metrics
  - Linux: `open()`, `read()`, `write()`, `close()`
  - Windows: `CreateFile()`, `ReadFile()`, `WriteFile()`, `CloseHandle()`

- **Process Management**: Process creation, monitoring, and termination
  - Linux: `fork()`, `exec()`, `wait()`
  - Windows: `CreateProcess()`, `WaitForSingleObject()`

- **Memory Management**: Dynamic memory allocation tracking
  - Linux: `mmap()`, `munmap()`
  - Windows: `VirtualAlloc()`, `VirtualFree()`

- **Inter-Process Communication**: IPC monitoring and analysis
  - Linux: `pipe()`, `shmget()`, `shmat()`
  - Windows: `CreatePipe()`, `CreateFileMapping()`, `MapViewOfFile()`

- **Permissions Management**:
  - Linux: `chmod()`, `chown()`, `umask()`
  - Windows: `SetFileSecurity()`

### Advanced Detection
- **FFT-Based Anomaly Detection**: Frequency analysis of system activity patterns
- **Real-time Monitoring**: Continuous system metrics collection
- **Attack Simulation**: Multiple attack scenarios for testing
  - Fork bomb (rapid process creation)
  - File spam (excessive file operations)
  - Memory exhaustion
  - IPC flooding

### Visualization Dashboard
- **Process Tree Visualization**: Real-time process hierarchy
- **Memory Usage Graphs**: Historical memory allocation tracking
- **File Activity Heatmap**: Visual representation of file operations
- **IPC Communication Flow**: Inter-process message tracking
- **FFT Frequency Spectrum**: Anomaly detection visualization
- **Alerts Panel**: Real-time security alerts

## 🏗️ Architecture

```
edr-system/
├── backend/                    # C++ system-level implementation
│   ├── src/
│   │   ├── core/              # Cross-platform abstraction layer
│   │   │   ├── file_handler   # File operations
│   │   │   ├── process_manager # Process management
│   │   │   ├── memory_manager # Memory operations
│   │   │   └── ipc_manager    # IPC operations
│   │   ├── monitoring/        # Detection and analysis
│   │   │   ├── monitor        # Main monitoring engine
│   │   │   ├── fft_engine     # FFT implementation
│   │   │   └── anomaly_detector # Rule-based detection
│   │   ├── simulation/        # Attack simulation
│   │   └── utils/             # Logging and performance
│   └── CMakeLists.txt
└── frontend/                   # React dashboard
    └── src/
        ├── components/        # UI components
        └── App.jsx           # Main application
```

## 🚀 Getting Started

### Prerequisites

**Backend (C++):**
- CMake 3.15+
- C++17 compatible compiler
  - Linux: GCC 7+ or Clang 5+
  - Windows: MSVC 2019+ or MinGW-w64

**Frontend (React):**
- Node.js 16+
- npm or yarn

### Building the Backend

**Linux:**
```bash
cd backend
chmod +x build.sh
./build.sh
./build/edr_system
```

**Windows (PowerShell):**
```powershell
cd backend
mkdir build
cd build
cmake ..
cmake --build . --config Release
.\Release\edr_system.exe
```

### Running the Frontend

```bash
cd frontend
npm install
npm run dev
```

Access the dashboard at `http://localhost:3000`

## 📊 Usage

### Backend CLI Menu

1. **Normal Mode**: Simulate regular system activity
2. **Attack Mode - Fork Bomb**: Rapid process creation
3. **Attack Mode - File Spam**: Excessive file operations
4. **Attack Mode - Memory Exhaustion**: Large memory allocations
5. **Attack Mode - IPC Flood**: High-volume IPC messages
6. **View Current Metrics**: Display system statistics
7. **View Alerts**: Show detected anomalies
8. **Export Logs (CSV)**: Export metrics to CSV
9. **Export Logs (JSON)**: Export metrics to JSON
10. **Generate Performance Report**: Create detailed analysis

### Dashboard Controls

- **Normal Mode**: View baseline system behavior
- **Attack Mode**: Simulate attacks and observe detection
- **Real-time Metrics**: Process count, memory, file ops, IPC, CPU
- **Interactive Visualizations**: Hover for details

## 🔬 Technical Implementation

### Cross-Platform Abstraction

The system uses a unified API that maps to platform-specific system calls:

```cpp
// Unified interface
FileHandle handle = fileHandler.open("test.txt", false);
fileHandler.write(handle, data, size, written);
fileHandler.close(handle);

// Internally maps to:
// Linux: open() -> write() -> close()
// Windows: CreateFile() -> WriteFile() -> CloseHandle()
```

### FFT Anomaly Detection

The system applies Fast Fourier Transform to system metrics:

1. Collect time-series data (process count, memory, etc.)
2. Apply Cooley-Tukey FFT algorithm
3. Analyze frequency spectrum
4. Detect anomalies based on magnitude thresholds
5. Trigger alerts for suspicious patterns

### Performance Metrics

All system calls are instrumented with:
- Execution time (milliseconds)
- Success/failure status
- Timestamp
- Error messages
- Resource usage

## 📈 Performance Analysis

The system generates comprehensive reports comparing:
- Average execution time per operation
- Success/failure rates
- Min/max execution times
- Platform-specific differences
- Resource consumption patterns

Example output:
```
Operation            Calls    Success  Failures  Avg (ms)  Min (ms)  Max (ms)
--------------------------------------------------------------------------------
open                 1250     1248     2         0.125     0.050     2.340
CreateProcess        45       45       0         2.450     1.200     5.670
mmap                 320      320      0         0.089     0.045     0.234
```

## 🛡️ Security Features

### Detection Capabilities

1. **Process Anomalies**: Fork bomb detection (>50 processes in 5s)
2. **Memory Anomalies**: Excessive allocation (>500MB threshold)
3. **File Anomalies**: Rapid file operations (>1000 ops/sec)
4. **IPC Anomalies**: Message flooding (>5000 msgs/sec)
5. **FFT Analysis**: Periodic attack patterns

### Alert System

- Real-time alert generation
- Severity classification
- Timestamp tracking
- Alert aggregation and filtering

## 📝 Logging

### Log Levels
- DEBUG: Detailed system call metrics
- INFO: General operational messages
- WARNING: Potential issues
- ERROR: Critical failures

### Export Formats
- **CSV**: Structured data for analysis
- **JSON**: Machine-readable format
- **TXT**: Human-readable reports

## 🧪 Testing

### Attack Simulations

The system includes built-in attack simulators:

```cpp
AttackSimulator simulator(monitor);

// Simulate various attacks
simulator.simulateForkBomb(100);
simulator.simulateFileSpam(".", 500);
simulator.simulateMemoryExhaustion(200);
simulator.simulateIPCFlood(10000);
```

### Validation

- Cross-platform consistency testing
- Performance benchmarking
- Anomaly detection accuracy
- False positive rate analysis

## 🔧 Configuration

### Anomaly Thresholds

Modify in `anomaly_detector.cpp`:
```cpp
const size_t PROCESS_THRESHOLD = 50;
const size_t MEMORY_THRESHOLD_MB = 500;
const size_t FILE_OPS_THRESHOLD = 1000;
const size_t IPC_THRESHOLD = 5000;
```

### FFT Parameters

Adjust in `fft_engine.cpp`:
```cpp
double threshold = 2.5;  // Standard deviations
size_t top_frequencies = 5;
```

## 📚 System Call Comparison

### File Operations
| Operation | Linux | Windows | Avg Time (Linux) | Avg Time (Windows) |
|-----------|-------|---------|------------------|-------------------|
| Open | open() | CreateFile() | ~0.12ms | ~0.18ms |
| Read | read() | ReadFile() | ~0.08ms | ~0.11ms |
| Write | write() | WriteFile() | ~0.09ms | ~0.13ms |
| Close | close() | CloseHandle() | ~0.05ms | ~0.07ms |

### Process Operations
| Operation | Linux | Windows | Avg Time (Linux) | Avg Time (Windows) |
|-----------|-------|---------|------------------|-------------------|
| Create | fork()+exec() | CreateProcess() | ~1.2ms | ~2.4ms |
| Wait | waitpid() | WaitForSingleObject() | ~0.3ms | ~0.5ms |

## 🎓 Educational Value

This project demonstrates:
- Operating system concepts (processes, memory, IPC)
- System programming (low-level APIs)
- Cross-platform development
- Signal processing (FFT)
- Real-time monitoring
- Security analysis
- Performance optimization
- Modern UI development

## 🤝 Contributing

Contributions welcome! Areas for enhancement:
- Additional attack patterns
- Machine learning integration
- Network monitoring
- Advanced visualization
- Mobile dashboard
- Distributed monitoring

## 📄 License

MIT License - See LICENSE file for details

## 🙏 Acknowledgments

- FFT implementation based on Cooley-Tukey algorithm
- UI inspired by modern security dashboards
- Cross-platform abstractions follow industry best practices

## 📞 Support

For issues, questions, or contributions:
- Open an issue on GitHub
- Review documentation in `/docs`
- Check examples in `/examples`

---

**Note**: This is an educational project. For production security monitoring, use established EDR solutions with comprehensive threat intelligence and enterprise support.
