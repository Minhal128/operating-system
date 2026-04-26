#include "monitoring/monitor.h"
#include "simulation/attack_simulator.h"
#include "utils/logger.h"
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef WINDOWS_PLATFORM
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

using namespace edr;

// Export metrics to JSON for frontend
void exportMetricsToJSON(Monitor& monitor, const std::string& filename) {
    auto metrics = monitor.getCurrentMetrics();
    auto alerts = monitor.getAlerts();
    auto processes = monitor.getProcessManager().getActiveProcesses();
    
    std::ofstream json(filename);
    if (!json.is_open()) return;
    
    json << "{\n";
    json << "  \"timestamp\": " << metrics.timestamp << ",\n";
    json << "  \"processCount\": " << metrics.process_count << ",\n";
    json << "  \"memoryAllocated\": " << metrics.memory_allocated << ",\n";
    json << "  \"systemMemoryUsage\": " << metrics.system_memory_usage << ",\n";
    json << "  \"fileOperations\": " << metrics.file_operations << ",\n";
    json << "  \"ipcMessages\": " << metrics.ipc_messages << ",\n";
    json << "  \"cpuUsage\": " << metrics.cpu_usage << ",\n";
    json << "  \"mode\": \"" << (monitor.getMode() == MonitorMode::NORMAL ? "NORMAL" : "ATTACK") << "\",\n";
    json << "  \"hasAnomalies\": " << (monitor.hasAnomalies() ? "true" : "false") << ",\n";
    
    // Processes
    json << "  \"processes\": [\n";
    for (size_t i = 0; i < processes.size(); ++i) {
        json << "    {\n";
        json << "      \"pid\": " << processes[i].pid << ",\n";
        json << "      \"name\": \"" << processes[i].name << "\",\n";
        json << "      \"running\": " << (processes[i].is_running ? "true" : "false") << "\n";
        json << "    }";
        if (i < processes.size() - 1) json << ",";
        json << "\n";
    }
    json << "  ],\n";
    
    // Alerts
    json << "  \"alerts\": [\n";
    for (size_t i = 0; i < alerts.size(); ++i) {
        json << "    \"" << alerts[i] << "\"";
        if (i < alerts.size() - 1) json << ",";
        json << "\n";
    }
    json << "  ]\n";
    
    json << "}\n";
    json.close();
}

int main() {
    std::cout << "=== EDR System API Server ===\n";
    std::cout << "Starting monitoring system...\n\n";
    
    Monitor monitor;
    AttackSimulator simulator(monitor);
    
    monitor.start();
    
    std::cout << "Server running. Exporting metrics to: public/api/metrics.json\n";
    std::cout << "Frontend can read from this file.\n\n";
    std::cout << "Commands:\n";
    std::cout << "  1 - Normal Mode\n";
    std::cout << "  2 - Fork Bomb Attack\n";
    std::cout << "  3 - File Spam Attack\n";
    std::cout << "  4 - Memory Exhaustion\n";
    std::cout << "  5 - IPC Flood\n";
    std::cout << "  0 - Exit\n\n";
    
    bool running = true;
    int choice;
    
    while (running) {
        // Continuously export metrics
        exportMetricsToJSON(monitor, "../frontend/public/api/metrics.json");
        
        // Check for user input (non-blocking would be better, but this is simple)
        std::cout << "Choice (or press Enter to continue monitoring): ";
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            monitor.collectMetrics();
            monitor.analyzeMetrics();
            SLEEP_MS(1000);
            continue;
        }
        
        choice = std::stoi(input);
        
        switch (choice) {
            case 1:
                std::cout << "Switching to Normal Mode\n";
                monitor.setMode(MonitorMode::NORMAL);
                simulator.simulateNormalActivity();
                break;
            case 2:
                std::cout << "Simulating Fork Bomb Attack\n";
                monitor.setMode(MonitorMode::ATTACK);
                simulator.simulateForkBomb(50);
                break;
            case 3:
                std::cout << "Simulating File Spam Attack\n";
                monitor.setMode(MonitorMode::ATTACK);
                simulator.simulateFileSpam(".", 200);
                break;
            case 4:
                std::cout << "Simulating Memory Exhaustion\n";
                monitor.setMode(MonitorMode::ATTACK);
                simulator.simulateMemoryExhaustion(100);
                break;
            case 5:
                std::cout << "Simulating IPC Flood\n";
                monitor.setMode(MonitorMode::ATTACK);
                simulator.simulateIPCFlood(5000);
                break;
            case 0:
                running = false;
                break;
            default:
                std::cout << "Invalid choice\n";
        }
        
        SLEEP_MS(500);
    }
    
    monitor.stop();
    std::cout << "\nAPI Server terminated.\n";
    return 0;
}
