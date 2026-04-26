#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "system_calls.h"
#include <vector>
#include <memory>

namespace edr {

class FileHandler {
public:
    FileHandler();
    ~FileHandler();

    // Cross-platform file operations
    FileHandle open(const std::string& path, bool read_only = true);
    bool read(FileHandle handle, void* buffer, size_t size, size_t& bytes_read);
    bool write(FileHandle handle, const void* buffer, size_t size, size_t& bytes_written);
    bool close(FileHandle handle);
    
    // Permission operations
    bool setPermissions(const std::string& path, uint32_t mode);
    bool setOwner(const std::string& path, uint32_t uid, uint32_t gid);
    
    // Metrics
    SystemCallMetrics getLastMetrics() const { return last_metrics_; }
    std::vector<SystemCallMetrics> getAllMetrics() const { return metrics_history_; }

private:
    SystemCallMetrics last_metrics_;
    std::vector<SystemCallMetrics> metrics_history_;
    
    void recordMetrics(const std::string& call_name, double exec_time, bool success, const std::string& error = "");
};

} // namespace edr

#endif // FILE_HANDLER_H
