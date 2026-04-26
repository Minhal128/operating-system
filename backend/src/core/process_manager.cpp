#include "process_manager.h"
#include <chrono>
#include <cstring>
#include <sstream>

#ifdef WINDOWS_PLATFORM
    #include <errno.h>
#else
    #include <cerrno>
#endif

#ifdef WINDOWS_PLATFORM
    #include <tlhelp32.h>
#else
    #include <sys/types.h>
    #include <signal.h>
#endif

namespace edr {

ProcessManager::ProcessManager() {}
ProcessManager::~ProcessManager() {}

ProcessHandle ProcessManager::create(const std::string& executable, const std::vector<std::string>& args) {
    auto start = std::chrono::high_resolution_clock::now();
    ProcessHandle handle = 0;
    bool success = false;
    std::string error;

#ifdef WINDOWS_PLATFORM
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    
    std::string cmdline = executable;
    for (const auto& arg : args) {
        cmdline += " " + arg;
    }
    
    success = CreateProcessA(NULL, const_cast<char*>(cmdline.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    
    if (success) {
        handle = pi.hProcess;
        addProcess(handle, pi.dwProcessId, executable);
        CloseHandle(pi.hThread);
    } else {
        error = "CreateProcess failed: " + std::to_string(GetLastError());
    }
#else
    pid_t pid = fork();
    
    if (pid < 0) {
        error = "fork() failed: " + std::string(strerror(errno));
        success = false;
    } else if (pid == 0) {
        // Child process
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(executable.c_str()));
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);
        
        execvp(executable.c_str(), argv.data());
        _exit(1); // If exec fails
    } else {
        // Parent process
        handle = pid;
        addProcess(handle, pid, executable);
        success = true;
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("create", exec_time, success, error);
    return handle;
}

bool ProcessManager::wait(ProcessHandle handle, int& exit_code) {
    auto start = std::chrono::high_resolution_clock::now();
    bool success = false;
    std::string error;
    exit_code = -1;

#ifdef WINDOWS_PLATFORM
    DWORD result = WaitForSingleObject(handle, INFINITE);
    if (result == WAIT_OBJECT_0) {
        DWORD code;
        if (GetExitCodeProcess(handle, &code)) {
            exit_code = static_cast<int>(code);
            success = true;
        } else {
            error = "GetExitCodeProcess failed: " + std::to_string(GetLastError());
        }
    } else {
        error = "WaitForSingleObject failed: " + std::to_string(GetLastError());
    }
#else
    int status;
    pid_t result = waitpid(handle, &status, 0);
    if (result > 0) {
        if (WIFEXITED(status)) {
            exit_code = WEXITSTATUS(status);
            success = true;
        } else {
            error = "Process did not exit normally";
        }
    } else {
        error = "waitpid() failed: " + std::string(strerror(errno));
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("wait", exec_time, success, error);
    removeProcess(handle);
    return success;
}

bool ProcessManager::terminate(ProcessHandle handle) {
    auto start = std::chrono::high_resolution_clock::now();
    bool success = false;
    std::string error;

#ifdef WINDOWS_PLATFORM
    success = TerminateProcess(handle, 1);
    if (!success) {
        error = "TerminateProcess failed: " + std::to_string(GetLastError());
    }
#else
    success = (kill(handle, SIGTERM) == 0);
    if (!success) {
        error = "kill() failed: " + std::string(strerror(errno));
    }
#endif

    auto end = std::chrono::high_resolution_clock::now();
    double exec_time = std::chrono::duration<double, std::milli>(end - start).count();
    
    recordMetrics("terminate", exec_time, success, error);
    removeProcess(handle);
    return success;
}

void ProcessManager::recordMetrics(const std::string& call_name, double exec_time, bool success, const std::string& error) {
    last_metrics_.call_name = call_name;
    last_metrics_.execution_time_ms = exec_time;
    last_metrics_.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    last_metrics_.success = success;
    last_metrics_.error_message = error;
    
    metrics_history_.push_back(last_metrics_);
}

void ProcessManager::addProcess(ProcessHandle handle, uint32_t pid, const std::string& name) {
    ProcessInfo info;
    info.handle = handle;
    info.pid = pid;
    info.name = name;
    info.creation_time = std::chrono::system_clock::now().time_since_epoch().count();
    info.is_running = true;
    
    active_processes_.push_back(info);
}

void ProcessManager::removeProcess(ProcessHandle handle) {
    for (auto it = active_processes_.begin(); it != active_processes_.end(); ++it) {
        if (it->handle == handle) {
            it->is_running = false;
            active_processes_.erase(it);
            break;
        }
    }
}

} // namespace edr
