#include "memory_manager.h"
#include <chrono>
#include <cstring>

#ifdef WINDOWS_PLATFORM
    #include <errno.h>
#else
    #include <cerrno>
#endif

namespace edr {

MemoryManager::MemoryManager() : total_allocated_(0) {}

MemoryManager::~MemoryManager() {
    // Clean up any remaining allocations
    for (auto& pair : allocations_) {
        if (pair.second.is_active) {
            free(pair.first, pair.second.size);
        }
    }
}

MemoryHandle MemoryManager::allocate(size_t size) {
    auto start = std::chrono::high_resolution_clock::now();
    MemoryHandle handle = nullptr;
    bool success = false;
    std::string error;

#ifdef WINDOWS_PLATFORM
    handle = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    success = (handle != NULL);
    if (!success) {
        error = "VirtualAlloc failed: " + std::to_string(GetLastError());
    }
#else
    handle = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    success = (handle != MAP_FAILED);
    if (!success) {
        error = "mmap() failed: " + std::string(strerror(errno));
        handle = nullptr;
    }
#endif

    if (success) {
        MemoryAllocation alloc;
        alloc.handle = handle;
        alloc.size = size;
        alloc.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        alloc.is_active = true;
        
        allocations_[handle] = alloc;
        total_allocated_ += size;
    }

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("allocate", exec_time, success, error);
    return handle;
}

bool MemoryManager::free(MemoryHandle handle, size_t size) {
    auto start = std::chrono::high_resolution_clock::now();
    bool success = false;
    std::string error;

    if (allocations_.find(handle) == allocations_.end()) {
        error = "Invalid memory handle";
    } else {
#ifdef WINDOWS_PLATFORM
        success = VirtualFree(handle, 0, MEM_RELEASE);
        if (!success) {
            error = "VirtualFree failed: " + std::to_string(GetLastError());
        }
#else
        success = (munmap(handle, size) == 0);
        if (!success) {
            error = "munmap() failed: " + std::string(strerror(errno));
        }
#endif

        if (success) {
            total_allocated_ -= allocations_[handle].size;
            allocations_[handle].is_active = false;
            allocations_.erase(handle);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("free", exec_time, success, error);
    return success;
}

std::vector<MemoryAllocation> MemoryManager::getActiveAllocations() const {
    std::vector<MemoryAllocation> active;
    for (const auto& pair : allocations_) {
        if (pair.second.is_active) {
            active.push_back(pair.second);
        }
    }
    return active;
}

void MemoryManager::recordMetrics(const std::string& call_name, double exec_time, bool success, const std::string& error) {
    last_metrics_.call_name = call_name;
    last_metrics_.execution_time_ms = exec_time;
    last_metrics_.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    last_metrics_.success = success;
    last_metrics_.error_message = error;
    
    metrics_history_.push_back(last_metrics_);
}

} // namespace edr
