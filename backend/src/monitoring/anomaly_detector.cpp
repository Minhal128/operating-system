#include "anomaly_detector.h"
#include <chrono>

namespace edr {

AnomalyDetector::AnomalyDetector() {}
AnomalyDetector::~AnomalyDetector() {}

bool AnomalyDetector::detectProcessAnomaly(size_t process_count, double time_window_sec) {
    // Fork bomb detection: > 50 processes in 5 seconds
    const size_t THRESHOLD = 50;
    const double TIME_THRESHOLD = 5.0;
    
    if (process_count > THRESHOLD && time_window_sec < TIME_THRESHOLD) {
        addReport("PROCESS_BOMB", 
                 "Excessive process creation detected: " + std::to_string(process_count) + " processes",
                 0.9);
        return true;
    }
    
    return false;
}

bool AnomalyDetector::detectMemoryAnomaly(size_t memory_allocated, size_t threshold_mb) {
    // Memory exhaustion detection
    size_t allocated_mb = memory_allocated / (1024 * 1024);
    
    if (allocated_mb > threshold_mb) {
        addReport("MEMORY_EXHAUSTION",
                 "Excessive memory allocation: " + std::to_string(allocated_mb) + " MB",
                 0.8);
        return true;
    }
    
    return false;
}

bool AnomalyDetector::detectFileAnomaly(size_t file_ops_per_sec) {
    // File spam detection: > 1000 ops/sec
    const size_t THRESHOLD = 1000;
    
    if (file_ops_per_sec > THRESHOLD) {
        addReport("FILE_SPAM",
                 "Excessive file operations: " + std::to_string(file_ops_per_sec) + " ops/sec",
                 0.7);
        return true;
    }
    
    return false;
}

bool AnomalyDetector::detectIPCAnomaly(size_t ipc_messages_per_sec) {
    // IPC flood detection: > 5000 messages/sec
    const size_t THRESHOLD = 5000;
    
    if (ipc_messages_per_sec > THRESHOLD) {
        addReport("IPC_FLOOD",
                 "Excessive IPC activity: " + std::to_string(ipc_messages_per_sec) + " msgs/sec",
                 0.75);
        return true;
    }
    
    return false;
}

void AnomalyDetector::addReport(const std::string& type, const std::string& desc, double severity) {
    AnomalyReport report;
    report.type = type;
    report.description = desc;
    report.severity = severity;
    report.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    
    reports_.push_back(report);
}

} // namespace edr
