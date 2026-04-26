#include "performance.h"
#include "logger.h"
#include <algorithm>
#include <numeric>
#include <fstream>
#include <iomanip>

namespace edr {

PerformanceAnalyzer::PerformanceAnalyzer() {}
PerformanceAnalyzer::~PerformanceAnalyzer() {}

void PerformanceAnalyzer::addMetrics(const std::vector<SystemCallMetrics>& metrics) {
    for (const auto& m : metrics) {
        operation_times_[m.call_name].push_back(m.execution_time_ms);
        
        if (m.success) {
            success_counts_[m.call_name]++;
        } else {
            failure_counts_[m.call_name]++;
        }
    }
}

std::vector<PerformanceStats> PerformanceAnalyzer::analyze() {
    std::vector<PerformanceStats> stats;
    
    for (const auto& pair : operation_times_) {
        const std::string& op = pair.first;
        const std::vector<double>& times = pair.second;
        
        if (times.empty()) continue;
        
        PerformanceStats s;
        s.operation = op;
        s.call_count = times.size();
        s.success_count = success_counts_[op];
        s.failure_count = failure_counts_[op];
        
        s.avg_time_ms = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
        s.min_time_ms = *std::min_element(times.begin(), times.end());
        s.max_time_ms = *std::max_element(times.begin(), times.end());
        
        stats.push_back(s);
    }
    
    return stats;
}

void PerformanceAnalyzer::generateReport(const std::string& filename) {
    auto stats = analyze();
    
    std::ofstream report(filename);
    if (!report.is_open()) {
        Logger::getInstance().log(LogLevel::LOG_ERROR, "Failed to open report file: " + filename);
        return;
    }
    
    report << "=== EDR System Performance Report ===\n\n";
    report << std::left << std::setw(20) << "Operation"
           << std::right << std::setw(12) << "Calls"
           << std::setw(12) << "Success"
           << std::setw(12) << "Failures"
           << std::setw(12) << "Avg (ms)"
           << std::setw(12) << "Min (ms)"
           << std::setw(12) << "Max (ms)" << "\n";
    report << std::string(92, '-') << "\n";
    
    for (const auto& s : stats) {
        report << std::left << std::setw(20) << s.operation
               << std::right << std::setw(12) << s.call_count
               << std::setw(12) << s.success_count
               << std::setw(12) << s.failure_count
               << std::setw(12) << std::fixed << std::setprecision(3) << s.avg_time_ms
               << std::setw(12) << s.min_time_ms
               << std::setw(12) << s.max_time_ms << "\n";
    }
    
    report.close();
    Logger::getInstance().log(LogLevel::INFO, "Performance report generated: " + filename);
}

void PerformanceAnalyzer::compareOperations(const std::string& op1, const std::string& op2) {
    auto it1 = operation_times_.find(op1);
    auto it2 = operation_times_.find(op2);
    
    if (it1 == operation_times_.end() || it2 == operation_times_.end()) {
        Logger::getInstance().log(LogLevel::WARNING, "Cannot compare: operations not found");
        return;
    }
    
    const auto& times1 = it1->second;
    const auto& times2 = it2->second;
    
    double avg1 = std::accumulate(times1.begin(), times1.end(), 0.0) / times1.size();
    double avg2 = std::accumulate(times2.begin(), times2.end(), 0.0) / times2.size();
    
    std::ostringstream oss;
    oss << "Comparison: " << op1 << " vs " << op2 << "\n"
        << "  " << op1 << ": " << avg1 << " ms (avg)\n"
        << "  " << op2 << ": " << avg2 << " ms (avg)\n"
        << "  Difference: " << (avg1 - avg2) << " ms";
    
    Logger::getInstance().log(LogLevel::INFO, oss.str());
}

} // namespace edr
