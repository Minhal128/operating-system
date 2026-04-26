#include "monitor.h"
#include <chrono>

#ifdef WINDOWS_PLATFORM
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0600
    #endif
    #include <windows.h>
#endif

#ifndef USE_SIMPLE_THREADING
    #include <thread>
#endif

namespace edr {

Monitor::Monitor() : running_(false), has_anomalies_(false), mode_(MonitorMode::NORMAL) {}

Monitor::~Monitor() {
    stop();
}

void Monitor::start() {
    if (running_) return;
    
    running_ = true;
#ifndef USE_SIMPLE_THREADING
    monitor_thread_ = std::thread(&Monitor::monitorLoop, this);
#endif
}

void Monitor::stop() {
    if (!running_) return;
    
    running_ = false;
#ifndef USE_SIMPLE_THREADING
    if (monitor_thread_.joinable()) {
        monitor_thread_.join();
    }
#endif
}

void Monitor::monitorLoop() {
#ifndef USE_SIMPLE_THREADING
    while (running_) {
        collectMetrics();
        analyzeMetrics();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
#endif
}

void Monitor::collectMetrics() {
    SystemMetrics metrics;
    metrics.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    metrics.process_count = process_manager_.getProcessCount();
    metrics.memory_allocated = memory_manager_.getTotalAllocated();
    metrics.file_operations = file_handler_.getAllMetrics().size();
    metrics.ipc_messages = ipc_manager_.getMessageCount();
    
    // Default values
    metrics.system_memory_usage = 0;
    metrics.cpu_usage = 0.0;

#ifdef WINDOWS_PLATFORM
    // Get real system memory
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo)) {
        // dwMemoryLoad is the percentage of physical memory in use
        // WorkingSet is also interesting, but dwMemoryLoad matches Task Manager's %
        metrics.system_memory_usage = (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / 1024; // in KB
        // We'll use the load percentage for CPU-like charts if needed, 
        // but for "Memory Usage" chart we usually want the value.
    }

    /*
    // Get real CPU usage (Simplified version using GetSystemTimes)
    static FILETIME lastIdleTime, lastKernelTime, lastUserTime;
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        auto ftTo64 = [](FILETIME ft) {
            return (uint64_t(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
        };
        
        uint64_t idle = ftTo64(idleTime) - ftTo64(lastIdleTime);
        uint64_t kernel = ftTo64(kernelTime) - ftTo64(lastKernelTime);
        uint64_t user = ftTo64(userTime) - ftTo64(lastUserTime);
        uint64_t total = kernel + user;
        
        if (total > 0) {
            metrics.cpu_usage = 100.0 * (total - idle) / total;
        }
        
        lastIdleTime = idleTime;
        lastKernelTime = kernelTime;
        lastUserTime = userTime;
    }
    */
    metrics.cpu_usage = 5.0; // Placeholder for now
#endif
    
    metrics_history_.push_back(metrics);
    
    // Keep last 1000 samples
    if (metrics_history_.size() > 1000) {
        metrics_history_.erase(metrics_history_.begin());
    }
}

void Monitor::analyzeMetrics() {
    if (metrics_history_.size() < 10) return;
    
    // Prepare data for FFT analysis
    std::vector<double> process_signal;
    std::vector<double> memory_signal;
    
    for (const auto& m : metrics_history_) {
        process_signal.push_back(static_cast<double>(m.process_count));
        memory_signal.push_back(static_cast<double>(m.memory_allocated));
    }
    
    // FFT analysis
    auto process_fft = fft_engine_.compute(process_signal);
    auto process_mag = fft_engine_.getMagnitudeSpectrum(process_fft);
    
    bool process_anomaly = fft_engine_.detectAnomaly(process_mag, 2.5);
    
    // Rule-based detection
    auto current = metrics_history_.back();
    bool mem_anomaly = anomaly_detector_.detectMemoryAnomaly(current.memory_allocated, 500);
    bool proc_anomaly = anomaly_detector_.detectProcessAnomaly(current.process_count, 5.0);
    
    has_anomalies_ = process_anomaly || mem_anomaly || proc_anomaly;
    
    if (has_anomalies_) {
        auto reports = anomaly_detector_.getReports();
        for (const auto& report : reports) {
            alerts_.push_back("[" + report.type + "] " + report.description);
        }
    }
}

std::vector<SystemMetrics> Monitor::getMetricsHistory() const {
    return metrics_history_;
}

SystemMetrics Monitor::getCurrentMetrics() const {
    if (metrics_history_.empty()) {
        return SystemMetrics{};
    }
    return metrics_history_.back();
}

std::vector<std::string> Monitor::getAlerts() const {
    return alerts_;
}

} // namespace edr
