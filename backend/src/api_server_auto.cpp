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

void printBanner() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "   EDR SYSTEM - API SERVER (AUTO)     \n";
    std::cout << "   Real-Time Intrusion Detection      \n";
    std::cout << "========================================\n";
    std::cout << "\n";
    std::cout.flush();
}

void printStatus(const std::string& message, const std::string& type = "INFO") {
    std::cout << "[" << type << "] " << message << std::endl;
    std::cout.flush();
}

// Export metrics to JSON for frontend
void exportMetricsToJSON(Monitor& monitor, const std::string& filename) {
    auto metrics = monitor.getCurrentMetrics();
    auto alerts = monitor.getAlerts();
    auto processes = monitor.getProcessManager().getActiveProcesses();
    
    std::ofstream json(filename);
    if (!json.is_open()) {
        std::cerr << "[ERROR] Failed to open: " << filename << std::endl;
        std::cerr.flush();
        return;
    }
    
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
    printBanner();
    
    printStatus("Initializing EDR System...", "INIT");
    Monitor monitor;
    AttackSimulator simulator(monitor);
    printStatus("Monitor initialized successfully", "OK");
    
    printStatus("Attack simulator ready", "OK");
    printStatus("Metrics export path: ../frontend/public/api/metrics.json", "INFO");
    printStatus("Frontend dashboard: http://localhost:3000", "INFO");
    
    std::cout << "\n";
    printStatus("Starting in NORMAL mode", "MODE");
    printStatus("Backend is now RUNNING - Collecting metrics...", "RUN");
    std::cout << "----------------------------------------\n\n";
    std::cout.flush();
    
    // Start in normal mode
    monitor.setMode(MonitorMode::NORMAL);
    
    int cycle = 0;
    bool firstExport = true;
    
    while (true) {
        // Collect metrics
        monitor.collectMetrics();
        monitor.analyzeMetrics();
        
        // Export to JSON
        exportMetricsToJSON(monitor, "../frontend/public/api/metrics.json");
        
        if (firstExport) {
            printStatus("First metrics export completed", "OK");
            printStatus("Dashboard should now show live data!", "INFO");
            std::cout << "\n";
            std::cout.flush();
            firstExport = false;
        }
        
        // Print detailed metrics every 5 seconds
        if (cycle % 5 == 0) {
            auto metrics = monitor.getCurrentMetrics();
            std::cout << "[TICK " << cycle << "s] ";
            std::cout << "Processes: " << metrics.process_count << " | ";
            std::cout << "Memory: " << (metrics.memory_allocated / 1024) << " KB | ";
            std::cout << "SystemMem: " << (metrics.system_memory_usage / 1024) << " MB | ";
            std::cout << "FileOps: " << metrics.file_operations << " | ";
            std::cout << "IPC: " << metrics.ipc_messages << " | ";
            std::cout << "CPU: " << metrics.cpu_usage << "% | ";
            std::cout << "Mode: " << (monitor.getMode() == MonitorMode::NORMAL ? "NORMAL" : "ATTACK");
            std::cout << std::endl;
            std::cout.flush();
        }
        
        // Simulate normal activity every 30 seconds
        if (cycle % 30 == 0 && cycle > 0) {
            std::cout << "\n";
            printStatus("Simulating normal system activity...", "SIM");
            simulator.simulateNormalActivity();
            printStatus("Normal activity simulation completed", "OK");
            std::cout << "\n";
            std::cout.flush();
        }
        
        // Switch to attack mode after 60 seconds for demo
        if (cycle == 60) {
            std::cout << "\n";
            std::cout << "****************************************\n";
            printStatus("DEMO: Switching to ATTACK mode", "ALERT");
            printStatus("Simulating Fork Bomb attack...", "ATTACK");
            std::cout << "****************************************\n";
            monitor.setMode(MonitorMode::ATTACK);
            simulator.simulateForkBomb(30);
            printStatus("Fork Bomb simulation completed", "OK");
            printStatus("Check dashboard for anomaly detection!", "INFO");
            std::cout << "\n";
            std::cout.flush();
        }
        
        // Switch back to normal after 90 seconds
        if (cycle == 90) {
            std::cout << "\n";
            std::cout << "****************************************\n";
            printStatus("DEMO: Switching back to NORMAL mode", "MODE");
            std::cout << "****************************************\n";
            monitor.setMode(MonitorMode::NORMAL);
            std::cout << "\n";
            std::cout.flush();
        }
        
        // Show alerts if any
        auto alerts = monitor.getAlerts();
        if (!alerts.empty() && cycle % 10 == 0) {
            std::cout << "\n";
            printStatus("Active Alerts: " + std::to_string(alerts.size()), "ALERT");
            for (size_t i = 0; i < std::min(alerts.size(), size_t(3)); ++i) {
                std::cout << "  -> " << alerts[i] << std::endl;
            }
            std::cout << "\n";
            std::cout.flush();
        }
        
        SLEEP_MS(1000);
        cycle++;
    }
    
    return 0;
}
