#include "file_handler.h"
#include <chrono>
#include <cstring>

#ifdef WINDOWS_PLATFORM
    #include <errno.h>
#else
    #include <cerrno>
#endif

namespace edr {

FileHandler::FileHandler() {}
FileHandler::~FileHandler() {}

FileHandle FileHandler::open(const std::string& path, bool read_only) {
    auto start = std::chrono::high_resolution_clock::now();
    FileHandle handle = INVALID_FILE_HANDLE;
    bool success = false;
    std::string error;

#ifdef WINDOWS_PLATFORM
    DWORD access = read_only ? GENERIC_READ : (GENERIC_READ | GENERIC_WRITE);
    DWORD share = FILE_SHARE_READ;
    DWORD creation = read_only ? OPEN_EXISTING : OPEN_ALWAYS;
    
    handle = CreateFileA(path.c_str(), access, share, NULL, creation, FILE_ATTRIBUTE_NORMAL, NULL);
    success = (handle != INVALID_HANDLE_VALUE);
    if (!success) {
        error = "CreateFile failed with error: " + std::to_string(GetLastError());
    }
#else
    int flags = read_only ? O_RDONLY : (O_RDWR | O_CREAT);
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    
    handle = ::open(path.c_str(), flags, mode);
    success = (handle != -1);
    if (!success) {
        error = "open() failed: " + std::string(strerror(errno));
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("open", exec_time, success, error);
    return handle;
}

bool FileHandler::read(FileHandle handle, void* buffer, size_t size, size_t& bytes_read) {
    auto start = std::chrono::high_resolution_clock::now();
    bool success = false;
    std::string error;
    bytes_read = 0;

#ifdef WINDOWS_PLATFORM
    DWORD read_bytes = 0;
    success = ReadFile(handle, buffer, static_cast<DWORD>(size), &read_bytes, NULL);
    bytes_read = read_bytes;
    if (!success) {
        error = "ReadFile failed with error: " + std::to_string(GetLastError());
    }
#else
    ssize_t result = ::read(handle, buffer, size);
    success = (result >= 0);
    if (success) {
        bytes_read = result;
    } else {
        error = "read() failed: " + std::string(strerror(errno));
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("read", exec_time, success, error);
    return success;
}

bool FileHandler::write(FileHandle handle, const void* buffer, size_t size, size_t& bytes_written) {
    auto start = std::chrono::high_resolution_clock::now();
    bool success = false;
    std::string error;
    bytes_written = 0;

#ifdef WINDOWS_PLATFORM
    DWORD written_bytes = 0;
    success = WriteFile(handle, buffer, static_cast<DWORD>(size), &written_bytes, NULL);
    bytes_written = written_bytes;
    if (!success) {
        error = "WriteFile failed with error: " + std::to_string(GetLastError());
    }
#else
    ssize_t result = ::write(handle, buffer, size);
    success = (result >= 0);
    if (success) {
        bytes_written = result;
    } else {
        error = "write() failed: " + std::string(strerror(errno));
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("write", exec_time, success, error);
    return success;
}

bool FileHandler::close(FileHandle handle) {
    auto start = std::chrono::high_resolution_clock::now();
    bool success = false;
    std::string error;

#ifdef WINDOWS_PLATFORM
    success = CloseHandle(handle);
    if (!success) {
        error = "CloseHandle failed with error: " + std::to_string(GetLastError());
    }
#else
    success = (::close(handle) == 0);
    if (!success) {
        error = "close() failed: " + std::string(strerror(errno));
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("close", exec_time, success, error);
    return success;
}

bool FileHandler::setPermissions(const std::string& path, uint32_t mode) {
    auto start = std::chrono::high_resolution_clock::now();
    bool success = false;
    std::string error;

#ifdef WINDOWS_PLATFORM
    // Windows uses ACLs - simplified implementation
    DWORD attributes = GetFileAttributesA(path.c_str());
    if (attributes != INVALID_FILE_ATTRIBUTES) {
        success = SetFileAttributesA(path.c_str(), attributes);
        if (!success) {
            error = "SetFileAttributes failed: " + std::to_string(GetLastError());
        }
    } else {
        error = "GetFileAttributes failed: " + std::to_string(GetLastError());
    }
#else
    success = (chmod(path.c_str(), mode) == 0);
    if (!success) {
        error = "chmod() failed: " + std::string(strerror(errno));
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("setPermissions", exec_time, success, error);
    return success;
}

bool FileHandler::setOwner(const std::string& path, uint32_t uid, uint32_t gid) {
    auto start = std::chrono::high_resolution_clock::now();
    bool success = false;
    std::string error;

#ifdef WINDOWS_PLATFORM
    // Windows ownership is complex - simplified
    error = "setOwner not fully implemented on Windows";
    success = false;
#else
    success = (chown(path.c_str(), uid, gid) == 0);
    if (!success) {
        error = "chown() failed: " + std::string(strerror(errno));
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("setOwner", exec_time, success, error);
    return success;
}

void FileHandler::recordMetrics(const std::string& call_name, double exec_time, bool success, const std::string& error) {
    last_metrics_.call_name = call_name;
    last_metrics_.execution_time_ms = exec_time;
    last_metrics_.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    last_metrics_.success = success;
    last_metrics_.error_message = error;
    
    metrics_history_.push_back(last_metrics_);
}

} // namespace edr
