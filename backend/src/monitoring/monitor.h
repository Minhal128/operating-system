#ifndef MONITOR_H
#define MONITOR_H

#include "../core/file_handler.h"
#include "../core/process_manager.h"
#include "../core/memory_manager.h"
#include "../core/ipc_manager.h"
#include "fft_engine.h"
#include "anomaly_detector.h"

#ifdef WINDOWS_PLATFORM
    #include <windows.h>
    #define USE_SIMPLE_THREADING
#else
    #include <thread>
    #include <mutex>
#endif

#include <atomic>
#include <vector>

namespace edr {

enum class MonitorMode {
    NORMAL,
    ATTACK
};

struct SystemMetrics {
    uint64_t timestamp;
    size_t process_count;
    size_t memory_allocated; // Still track internal for EDR purposes
    size_t system_memory_usage; // Real system-wide usage in KB
    size_t file_operations;
    size_t ipc_messages;
    double cpu_usage; // System-wide CPU usage %
};

class Monitor {
public:
    Monitor();
    ~Monitor();

    // Control
    void start();
    void stop();
    void setMode(MonitorMode mode) { mode_ = mode; }
    MonitorMode getMode() const { return mode_; }
    
    // Manual metric collection (for API server)
    void collectMetrics();
    void analyzeMetrics();
    
    // Component access
    FileHandler& getFileHandler() { return file_handler_; }
    ProcessManager& getProcessManager() { return process_manager_; }
    MemoryManager& getMemoryManager() { return memory_manager_; }
    IPCManager& getIPCManager() { return ipc_manager_; }
    
    // Metrics
    std::vector<SystemMetrics> getMetricsHistory() const;
    SystemMetrics getCurrentMetrics() const;
    
    // Anomaly detection
    bool hasAnomalies() const { return has_anomalies_; }
    std::vector<std::string> getAlerts() const;

private:
    FileHandler file_handler_;
    ProcessManager process_manager_;
    MemoryManager memory_manager_;
    IPCManager ipc_manager_;
    FFTEngine fft_engine_;
    AnomalyDetector anomaly_detector_;
    
#ifndef USE_SIMPLE_THREADING
    std::thread monitor_thread_;
    std::mutex metrics_mutex_;
#endif
    
    std::atomic<bool> running_;
    std::atomic<bool> has_anomalies_;
    MonitorMode mode_;
    
    std::vector<SystemMetrics> metrics_history_;
    std::vector<std::string> alerts_;
    
    void monitorLoop();
};

} // namespace edr

#endif // MONITOR_H
