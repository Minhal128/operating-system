#include "ipc_manager.h"
#include <chrono>
#include <cstring>

#ifdef WINDOWS_PLATFORM
    #include <errno.h>
#else
    #include <cerrno>
#endif

#ifndef WINDOWS_PLATFORM
    #include <sys/shm.h>
#endif

namespace edr {

IPCManager::IPCManager() {}
IPCManager::~IPCManager() {}

bool IPCManager::createPipe(PipeHandle& read_handle, PipeHandle& write_handle) {
    auto start = std::chrono::high_resolution_clock::now();
    bool success = false;
    std::string error;

#ifdef WINDOWS_PLATFORM
    SECURITY_ATTRIBUTES sa = {0};
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    
    success = CreatePipe(&read_handle, &write_handle, &sa, 0);
    if (!success) {
        error = "CreatePipe failed: " + std::to_string(GetLastError());
    }
#else
    int fds[2];
    success = (pipe(fds) == 0);
    if (success) {
        read_handle = fds[0];
        write_handle = fds[1];
    } else {
        error = "pipe() failed: " + std::string(strerror(errno));
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("createPipe", exec_time, success, error);
    return success;
}

bool IPCManager::writePipe(PipeHandle handle, const void* data, size_t size) {
    auto start = std::chrono::high_resolution_clock::now();
    bool success = false;
    std::string error;

#ifdef WINDOWS_PLATFORM
    DWORD written;
    success = WriteFile(handle, data, static_cast<DWORD>(size), &written, NULL);
    if (!success) {
        error = "WriteFile failed: " + std::to_string(GetLastError());
    }
#else
    ssize_t result = write(handle, data, size);
    success = (result >= 0);
    if (!success) {
        error = "write() failed: " + std::string(strerror(errno));
    }
#endif

    if (success) {
        recordMessage(size);
    }

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("writePipe", exec_time, success, error);
    return success;
}

bool IPCManager::readPipe(PipeHandle handle, void* buffer, size_t size, size_t& bytes_read) {
    auto start = std::chrono::high_resolution_clock::now();
    bool success = false;
    std::string error;
    bytes_read = 0;

#ifdef WINDOWS_PLATFORM
    DWORD read_bytes;
    success = ReadFile(handle, buffer, static_cast<DWORD>(size), &read_bytes, NULL);
    bytes_read = read_bytes;
    if (!success) {
        error = "ReadFile failed: " + std::to_string(GetLastError());
    }
#else
    ssize_t result = read(handle, buffer, size);
    success = (result >= 0);
    if (success) {
        bytes_read = result;
    } else {
        error = "read() failed: " + std::string(strerror(errno));
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("readPipe", exec_time, success, error);
    return success;
}

bool IPCManager::closePipe(PipeHandle handle) {
    auto start = std::chrono::high_resolution_clock::now();
    bool success = false;
    std::string error;

#ifdef WINDOWS_PLATFORM
    success = CloseHandle(handle);
    if (!success) {
        error = "CloseHandle failed: " + std::to_string(GetLastError());
    }
#else
    success = (close(handle) == 0);
    if (!success) {
        error = "close() failed: " + std::string(strerror(errno));
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("closePipe", exec_time, success, error);
    return success;
}

SharedMemHandle IPCManager::createSharedMemory(const std::string& name, size_t size) {
    auto start = std::chrono::high_resolution_clock::now();
    SharedMemHandle handle = 0;
    bool success = false;
    std::string error;

#ifdef WINDOWS_PLATFORM
    handle = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, static_cast<DWORD>(size), name.c_str());
    success = (handle != NULL);
    if (!success) {
        error = "CreateFileMapping failed: " + std::to_string(GetLastError());
    }
#else
    key_t key = ftok(name.c_str(), 65);
    handle = shmget(key, size, 0666 | IPC_CREAT);
    success = (handle != -1);
    if (!success) {
        error = "shmget() failed: " + std::string(strerror(errno));
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("createSharedMemory", exec_time, success, error);
    return handle;
}

void* IPCManager::mapSharedMemory(SharedMemHandle handle, size_t size) {
    auto start = std::chrono::high_resolution_clock::now();
    void* addr = nullptr;
    bool success = false;
    std::string error;

#ifdef WINDOWS_PLATFORM
    addr = MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, size);
    success = (addr != NULL);
    if (!success) {
        error = "MapViewOfFile failed: " + std::to_string(GetLastError());
    }
#else
    addr = shmat(handle, NULL, 0);
    success = (addr != (void*)-1);
    if (!success) {
        error = "shmat() failed: " + std::string(strerror(errno));
        addr = nullptr;
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("mapSharedMemory", exec_time, success, error);
    return addr;
}

bool IPCManager::unmapSharedMemory(void* addr, size_t size) {
    auto start = std::chrono::high_resolution_clock::now();
    bool success = false;
    std::string error;

#ifdef WINDOWS_PLATFORM
    success = UnmapViewOfFile(addr);
    if (!success) {
        error = "UnmapViewOfFile failed: " + std::to_string(GetLastError());
    }
#else
    success = (shmdt(addr) == 0);
    if (!success) {
        error = "shmdt() failed: " + std::string(strerror(errno));
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("unmapSharedMemory", exec_time, success, error);
    return success;
}

bool IPCManager::closeSharedMemory(SharedMemHandle handle) {
    auto start = std::chrono::high_resolution_clock::now();
    bool success = false;
    std::string error;

#ifdef WINDOWS_PLATFORM
    success = CloseHandle(handle);
    if (!success) {
        error = "CloseHandle failed: " + std::to_string(GetLastError());
    }
#else
    success = (shmctl(handle, IPC_RMID, NULL) == 0);
    if (!success) {
        error = "shmctl() failed: " + std::string(strerror(errno));
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("closeSharedMemory", exec_time, success, error);
    return success;
}

void IPCManager::recordMetrics(const std::string& call_name, double exec_time, bool success, const std::string& error) {
    last_metrics_.call_name = call_name;
    last_metrics_.execution_time_ms = exec_time;
    last_metrics_.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    last_metrics_.success = success;
    last_metrics_.error_message = error;
    
    metrics_history_.push_back(last_metrics_);
}

void IPCManager::recordMessage(size_t size) {
    IPCMessage msg;
    msg.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    msg.size = size;
    msg.source = "process";
    msg.destination = "pipe";
    
    message_history_.push_back(msg);
}

} // namespace edr
