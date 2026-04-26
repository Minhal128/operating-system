#include "attack_simulator.h"
#include <iostream>

#ifdef WINDOWS_PLATFORM
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <thread>
    #include <chrono>
    #define SLEEP_MS(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))
#endif

namespace edr {

AttackSimulator::AttackSimulator(Monitor& monitor) : monitor_(monitor) {}
AttackSimulator::~AttackSimulator() {}

void AttackSimulator::simulateForkBomb(size_t process_count) {
    std::cout << "[ATTACK] Simulating fork bomb with " << process_count << " processes\n";
    
    auto& pm = monitor_.getProcessManager();
    
    for (size_t i = 0; i < process_count; ++i) {
#ifdef WINDOWS_PLATFORM
        pm.create("cmd.exe", {"/c", "echo", "test"});
#else
        pm.create("/bin/echo", {"test"});
#endif
        SLEEP_MS(10);
    }
}

void AttackSimulator::simulateFileSpam(const std::string& directory, size_t file_count) {
    std::cout << "[ATTACK] Simulating file spam with " << file_count << " files\n";
    
    auto& fh = monitor_.getFileHandler();
    
    for (size_t i = 0; i < file_count; ++i) {
        std::string filename = directory + "/test_" + std::to_string(i) + ".tmp";
        auto handle = fh.open(filename, false);
        
        if (handle != INVALID_FILE_HANDLE) {
            std::string data = "Test data " + std::to_string(i);
            size_t written;
            fh.write(handle, data.c_str(), data.size(), written);
            fh.close(handle);
        }
        
        if (i % 100 == 0) {
            SLEEP_MS(1);
        }
    }
}

void AttackSimulator::simulateMemoryExhaustion(size_t mb_to_allocate) {
    std::cout << "[ATTACK] Simulating memory exhaustion: " << mb_to_allocate << " MB\n";
    
    auto& mm = monitor_.getMemoryManager();
    
    size_t chunk_size = 1024 * 1024; // 1 MB chunks
    size_t chunks = mb_to_allocate;
    
    for (size_t i = 0; i < chunks; ++i) {
        mm.allocate(chunk_size);
        SLEEP_MS(5);
    }
}

void AttackSimulator::simulateIPCFlood(size_t message_count) {
    std::cout << "[ATTACK] Simulating IPC flood with " << message_count << " messages\n";
    
    auto& ipc = monitor_.getIPCManager();
    
    PipeHandle read_handle, write_handle;
    if (ipc.createPipe(read_handle, write_handle)) {
        for (size_t i = 0; i < message_count; ++i) {
            std::string msg = "Message " + std::to_string(i);
            ipc.writePipe(write_handle, msg.c_str(), msg.size());
            
            if (i % 1000 == 0) {
                SLEEP_MS(1);
            }
        }
        
        ipc.closePipe(read_handle);
        ipc.closePipe(write_handle);
    }
}

void AttackSimulator::simulateNormalActivity() {
    std::cout << "[NORMAL] Simulating normal system activity\n";
    
    auto& fh = monitor_.getFileHandler();
    auto& mm = monitor_.getMemoryManager();
    
    // Normal file operations
    for (int i = 0; i < 5; ++i) {
        std::string filename = "normal_file_" + std::to_string(i) + ".txt";
        auto handle = fh.open(filename, false);
        if (handle != INVALID_FILE_HANDLE) {
            std::string data = "Normal data";
            size_t written;
            fh.write(handle, data.c_str(), data.size(), written);
            fh.close(handle);
        }
        SLEEP_MS(500);
    }
    
    // Normal memory allocation
    for (int i = 0; i < 3; ++i) {
        auto mem = mm.allocate(1024 * 100); // 100 KB
        SLEEP_MS(300);
        if (mem) {
            mm.free(mem, 1024 * 100);
        }
    }
}

} // namespace edr
