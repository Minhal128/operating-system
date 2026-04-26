#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "system_calls.h"
#include <vector>
#include <string>

namespace edr {

struct ProcessInfo {
    ProcessHandle handle;
    uint32_t pid;
    std::string name;
    uint64_t creation_time;
    bool is_running;
};

class ProcessManager {
public:
    ProcessManager();
    ~ProcessManager();

    // Cross-platform process operations
    ProcessHandle create(const std::string& executable, const std::vector<std::string>& args);
    bool wait(ProcessHandle handle, int& exit_code);
    bool terminate(ProcessHandle handle);
    
    // Process monitoring
    std::vector<ProcessInfo> getActiveProcesses() const { return active_processes_; }
    size_t getProcessCount() const { return active_processes_.size(); }
    
    // Metrics
    SystemCallMetrics getLastMetrics() const { return last_metrics_; }
    std::vector<SystemCallMetrics> getAllMetrics() const { return metrics_history_; }

private:
    std::vector<ProcessInfo> active_processes_;
    SystemCallMetrics last_metrics_;
    std::vector<SystemCallMetrics> metrics_history_;
    
    void recordMetrics(const std::string& call_name, double exec_time, bool success, const std::string& error = "");
    void addProcess(ProcessHandle handle, uint32_t pid, const std::string& name);
    void removeProcess(ProcessHandle handle);
};

} // namespace edr

#endif // PROCESS_MANAGER_H
