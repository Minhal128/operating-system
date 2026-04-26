#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "system_calls.h"
#include <vector>
#include <map>

namespace edr {

struct MemoryAllocation {
    MemoryHandle handle;
    size_t size;
    uint64_t timestamp;
    bool is_active;
};

class MemoryManager {
public:
    MemoryManager();
    ~MemoryManager();

    // Cross-platform memory operations
    MemoryHandle allocate(size_t size);
    bool free(MemoryHandle handle, size_t size);
    
    // Memory monitoring
    size_t getTotalAllocated() const { return total_allocated_; }
    size_t getAllocationCount() const { return allocations_.size(); }
    std::vector<MemoryAllocation> getActiveAllocations() const;
    
    // Metrics
    SystemCallMetrics getLastMetrics() const { return last_metrics_; }
    std::vector<SystemCallMetrics> getAllMetrics() const { return metrics_history_; }

private:
    std::map<MemoryHandle, MemoryAllocation> allocations_;
    size_t total_allocated_;
    SystemCallMetrics last_metrics_;
    std::vector<SystemCallMetrics> metrics_history_;
    
    void recordMetrics(const std::string& call_name, double exec_time, bool success, const std::string& error = "");
};

} // namespace edr

#endif // MEMORY_MANAGER_H
