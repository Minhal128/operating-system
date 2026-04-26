# Quick Start Guide

## 🚀 Get Running in 5 Minutes

### Step 1: Build the Backend

**On Linux:**
```bash
cd backend
chmod +x build.sh
./build.sh
```

**On Windows:**
```powershell
cd backend
.\build.bat
```

### Step 2: Run the Backend

**Linux:**
```bash
./build/edr_system
```

**Windows:**
```powershell
.\build\Release\edr_system.exe
```

### Step 3: Start the Dashboard

Open a new terminal:

```bash
cd frontend
npm install
npm run dev
```

Open browser to: `http://localhost:3000`

### Step 4: Try It Out!

In the backend terminal, select from the menu:

1. **Start with Normal Mode** (Option 1)
   - Watch the dashboard show normal activity
   - Observe baseline metrics

2. **Switch to Attack Mode** (Option 2-5)
   - Try Fork Bomb (Option 2)
   - Watch alerts appear in real-time
   - See FFT spectrum detect anomalies

3. **Export Results** (Option 8-10)
   - Generate CSV logs
   - Create performance reports
   - Analyze system behavior

## 📊 What You'll See

### Dashboard Features

1. **Top Metrics Bar**
   - Process count
   - Memory usage
   - File operations
   - IPC messages
   - CPU usage

2. **Process Tree**
   - Active processes
   - Suspicious processes highlighted in red
   - Real-time updates

3. **Memory Chart**
   - Historical memory usage
   - Smooth line graph
   - Auto-scaling

4. **File Heatmap**
   - 10x10 grid of file activity
   - Blue = normal, Red = attack
   - Intensity shows activity level

5. **IPC Flow**
   - Process communication paths
   - Message counts
   - Directional arrows

6. **FFT Spectrum**
   - Frequency analysis
   - Anomaly detection
   - Red bars = suspicious patterns

7. **Alerts Panel**
   - Real-time security alerts
   - Severity indicators
   - Clear all button

## 🎯 Try These Scenarios

### Scenario 1: Normal Operations
```
Backend Menu → Option 1 (Normal Mode)
Dashboard → Switch to "Normal Mode"
```
**Expected:** Low, steady metrics. No alerts.

### Scenario 2: Fork Bomb Attack
```
Backend Menu → Option 2 (Fork Bomb)
Dashboard → Switch to "Attack Mode"
```
**Expected:** 
- Process count spikes
- Red processes in tree
- "PROCESS_BOMB" alerts
- FFT shows anomaly

### Scenario 3: Memory Exhaustion
```
Backend Menu → Option 4 (Memory Exhaustion)
Dashboard → Watch memory chart
```
**Expected:**
- Memory usage climbs rapidly
- "MEMORY_EXHAUSTION" alert
- Chart shows steep increase

### Scenario 4: File Spam
```
Backend Menu → Option 3 (File Spam)
Dashboard → Watch file heatmap
```
**Expected:**
- Heatmap turns red
- File ops counter increases
- "FILE_SPAM" alert

## 📈 Understanding the Output

### Backend Console

```
=== EDR System - Real-Time Intrusion Detection ===
1. Start Normal Mode
2. Start Attack Mode - Fork Bomb
...
Choice: 2

[ATTACK] Simulating fork bomb with 100 processes
[WARNING] Starting Fork Bomb Attack
```

### Log Files

**edr_system.log:**
```
[2024-01-15 10:30:45.123] [INFO] EDR System Starting...
[2024-01-15 10:30:46.456] [WARNING] Starting Fork Bomb Attack
[2024-01-15 10:30:47.789] [DEBUG] METRIC: create | Time: 2.45ms | Success: YES
```

**edr_metrics.csv:**
```csv
Timestamp,CallName,ExecutionTime_ms,Success,ErrorMessage
1705315845123,open,0.125,true,""
1705315845456,create,2.450,true,""
```

**performance_report.txt:**
```
=== EDR System Performance Report ===

Operation            Calls    Success  Failures  Avg (ms)  Min (ms)  Max (ms)
--------------------------------------------------------------------------------
open                 1250     1248     2         0.125     0.050     2.340
create               45       45       0         2.450     1.200     5.670
```

## 🔧 Configuration

### Adjust Detection Thresholds

Edit `backend/src/monitoring/anomaly_detector.cpp`:

```cpp
// Line 10-13
const size_t PROCESS_THRESHOLD = 50;      // Processes in 5 sec
const size_t MEMORY_THRESHOLD_MB = 500;   // MB allocated
const size_t FILE_OPS_THRESHOLD = 1000;   // Ops per second
const size_t IPC_THRESHOLD = 5000;        // Messages per second
```

Rebuild after changes:
```bash
cd backend/build
cmake --build . --config Release
```

### Adjust FFT Sensitivity

Edit `backend/src/monitoring/fft_engine.cpp`:

```cpp
// Line 85
double threshold = 2.5;  // Standard deviations (lower = more sensitive)
```

### Change Monitoring Frequency

Edit `backend/src/monitoring/monitor.cpp`:

```cpp
// Line 25
std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 100ms = 10Hz
```

## 🐛 Troubleshooting

### Backend won't build

**Linux:**
```bash
# Install dependencies
sudo apt-get install build-essential cmake

# Check compiler
g++ --version  # Should be 7.0+
```

**Windows:**
```powershell
# Install Visual Studio 2019 or later
# Or install MinGW-w64
```

### Frontend won't start

```bash
# Clear cache and reinstall
rm -rf node_modules package-lock.json
npm install

# Try different port
npm run dev -- --port 3001
```

### Permission errors (Linux)

```bash
# Some operations need root
sudo ./build/edr_system

# Or adjust file permissions
chmod 755 ./build/edr_system
```

### No alerts appearing

1. Make sure you're in "Attack Mode" on dashboard
2. Check backend is running attack simulation
3. Verify thresholds aren't too high
4. Check console for errors

## 📚 Next Steps

1. **Read the full README.md** for detailed features
2. **Check IMPLEMENTATION_GUIDE.md** for architecture details
3. **Experiment with different attacks** and observe patterns
4. **Modify thresholds** to see how detection changes
5. **Export and analyze logs** to understand system behavior

## 💡 Tips

- **Run attacks one at a time** to see clear patterns
- **Let normal mode run** for a baseline before attacks
- **Export logs regularly** to track changes over time
- **Watch the FFT spectrum** - it's the most interesting visualization!
- **Try multiple attacks** in sequence to see cumulative effects

## 🎓 Learning Objectives

By using this system, you'll understand:

1. **System Calls**: How OS operations work at low level
2. **Cross-Platform Development**: Differences between Linux/Windows
3. **Real-Time Monitoring**: Collecting and analyzing live data
4. **Anomaly Detection**: Using FFT for pattern recognition
5. **Security Analysis**: Identifying malicious behavior
6. **Performance Metrics**: Measuring system call overhead
7. **Visualization**: Presenting complex data clearly

## 🤝 Need Help?

- Check the logs: `edr_system.log`
- Review error messages in console
- Verify all dependencies installed
- Try rebuilding from scratch
- Check GitHub issues (if applicable)

---

**Happy Monitoring! 🛡️**
