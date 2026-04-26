#ifndef SYSTEM_CALLS_H
#define SYSTEM_CALLS_H

#include <string>
#include <cstdint>

#ifdef WINDOWS_PLATFORM
    #include <windows.h>
#else
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/mman.h>
    #include <sys/wait.h>
    #include <sys/stat.h>
    #include <sys/ipc.h>
    #include <sys/shm.h>
#endif

namespace edr {

// Cross-platform file handle
#ifdef WINDOWS_PLATFORM
    typedef HANDLE FileHandle;
    typedef HANDLE ProcessHandle;
    typedef LPVOID MemoryHandle;
    #define INVALID_FILE_HANDLE INVALID_HANDLE_VALUE
#else
    typedef int FileHandle;
    typedef pid_t ProcessHandle;
    typedef void* MemoryHandle;
    #define INVALID_FILE_HANDLE -1
#endif

struct SystemCallMetrics {
    std::string call_name;
    double execution_time_ms;
    uint64_t timestamp;
    bool success;
    std::string error_message;
};

} // namespace edr

#endif // SYSTEM_CALLS_H
