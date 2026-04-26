#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <string>
#include <vector>
#include <map>
#include "../core/system_calls.h"

namespace edr {

struct PerformanceStats {
    std::string operation;
    double avg_time_ms;
    double min_time_ms;
    double max_time_ms;
    size_t call_count;
    size_t success_count;
    size_t failure_count;
};

class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();

    void addMetrics(const std::vector<SystemCallMetrics>& metrics);
    std::vector<PerformanceStats> analyze();
    void generateReport(const std::string& filename);
    
    void compareOperations(const std::string& op1, const std::string& op2);

private:
    std::map<std::string, std::vector<double>> operation_times_;
    std::map<std::string, size_t> success_counts_;
    std::map<std::string, size_t> failure_counts_;
};

} // namespace edr

#endif // PERFORMANCE_H
