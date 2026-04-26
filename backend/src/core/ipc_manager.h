#ifndef IPC_MANAGER_H
#define IPC_MANAGER_H

#include "system_calls.h"
#include <vector>
#include <string>

namespace edr {

#ifdef WINDOWS_PLATFORM
    typedef HANDLE PipeHandle;
    typedef HANDLE SharedMemHandle;
#else
    typedef int PipeHandle;
    typedef int SharedMemHandle;
#endif

struct IPCMessage {
    uint64_t timestamp;
    size_t size;
    std::string source;
    std::string destination;
};

class IPCManager {
public:
    IPCManager();
    ~IPCManager();

    // Pipe operations
    bool createPipe(PipeHandle& read_handle, PipeHandle& write_handle);
    bool writePipe(PipeHandle handle, const void* data, size_t size);
    bool readPipe(PipeHandle handle, void* buffer, size_t size, size_t& bytes_read);
    bool closePipe(PipeHandle handle);
    
    // Shared memory operations
    SharedMemHandle createSharedMemory(const std::string& name, size_t size);
    void* mapSharedMemory(SharedMemHandle handle, size_t size);
    bool unmapSharedMemory(void* addr, size_t size);
    bool closeSharedMemory(SharedMemHandle handle);
    
    // Monitoring
    std::vector<IPCMessage> getMessageHistory() const { return message_history_; }
    size_t getMessageCount() const { return message_history_.size(); }
    
    // Metrics
    SystemCallMetrics getLastMetrics() const { return last_metrics_; }
    std::vector<SystemCallMetrics> getAllMetrics() const { return metrics_history_; }

private:
    std::vector<IPCMessage> message_history_;
    SystemCallMetrics last_metrics_;
    std::vector<SystemCallMetrics> metrics_history_;
    
    void recordMetrics(const std::string& call_name, double exec_time, bool success, const std::string& error = "");
    void recordMessage(size_t size);
};

} // namespace edr

#endif // IPC_MANAGER_H
