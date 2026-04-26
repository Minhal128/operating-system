#ifndef ANOMALY_DETECTOR_H
#define ANOMALY_DETECTOR_H

#include <vector>
#include <string>

namespace edr {

struct AnomalyReport {
    std::string type;
    std::string description;
    double severity;
    uint64_t timestamp;
};

class AnomalyDetector {
public:
    AnomalyDetector();
    ~AnomalyDetector();

    // Detection methods
    bool detectProcessAnomaly(size_t process_count, double time_window_sec);
    bool detectMemoryAnomaly(size_t memory_allocated, size_t threshold_mb);
    bool detectFileAnomaly(size_t file_ops_per_sec);
    bool detectIPCAnomaly(size_t ipc_messages_per_sec);
    
    // Get reports
    std::vector<AnomalyReport> getReports() const { return reports_; }
    void clearReports() { reports_.clear(); }

private:
    std::vector<AnomalyReport> reports_;
    
    void addReport(const std::string& type, const std::string& desc, double severity);
};

} // namespace edr

#endif // ANOMALY_DETECTOR_H
