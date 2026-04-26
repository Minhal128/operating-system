#include "monitoring/monitor.h"
#include "simulation/attack_simulator.h"
#include <iostream>
#include <fstream>
#include <ctime>

#ifdef WINDOWS_PLATFORM
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

using namespace edr;

// Log to both console and file
std::ofstream logFile;

void log(const std::string& message) {
    time_t now = time(0);
    char timestamp[100];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    std::string logMsg = std::string("[") + timestamp + "] " + message;
    
    // Console
    std::cout << logMsg << std::endl;
    
    // File
    if (logFile.is_open()) {
        logFile << logMsg << std::endl;
        logFile.flush();
    }
}

void exportMetrics(Monitor& monitor) {
    auto metrics = monitor.getCurrentMetrics();
    auto alerts = monitor.getAlerts();
    auto processes = monitor.getProcessManager().getActiveProcesses();
    
    std::ofstream json("../frontend/public/api/metrics.json");
    if (!json.is_open()) {
        log("[ERROR] Cannot create metrics.json");
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
    json << "  \"processes\": [\n";
    
    for (size_t i = 0; i < processes.size(); ++i) {
        json << "    {\"pid\": " << processes[i].pid << ", \"name\": \"" << processes[i].name << "\", \"running\": " << (processes[i].is_running ? "true" : "false") << "}";
        if (i < processes.size() - 1) json << ",";
        json << "\n";
    }
    
    json << "  ],\n  \"alerts\": [\n";
    
    for (size_t i = 0; i < alerts.size(); ++i) {
        json << "    \"" << alerts[i] << "\"";
        if (i < alerts.size() - 1) json << ",";
        json << "\n";
    }
    
    json << "  ]\n}\n";
    json.close();
}

int main() {
    // Open log file
    logFile.open("edr_backend.log", std::ios::app);
    
    log("========================================");
    log("  EDR SYSTEM - BACKEND API SERVER");
    log("  Real-Time Intrusion Detection");
    log("========================================");
    log("");
    
    log("[INIT] Starting EDR System...");
    
    Monitor monitor;
    AttackSimulator simulator(monitor);
    
    log("[OK] Monitor initialized");
    log("[OK] Attack simulator ready");
    log("[INFO] Metrics file: ../frontend/public/api/metrics.json");
    log("[INFO] Dashboard: http://localhost:3000");
    log("[INFO] Log file: edr_backend.log");
    log("");
    log("[MODE] Starting in NORMAL mode");
    log("[RUN] Backend is RUNNING - Collecting metrics every second");
    log("========================================");
    log("");
    
    monitor.setMode(MonitorMode::NORMAL);
    
    int cycle = 0;
    bool firstExport = true;
    
    while (true) {
        // Collect and export
        monitor.collectMetrics();
        monitor.analyzeMetrics();
        exportMetrics(monitor);
        
        if (firstExport) {
            log("[OK] First metrics export completed!");
            log("[INFO] Dashboard should now display live data");
            log("");
            firstExport = false;
        }
        
        // Log every 5 seconds
        if (cycle % 5 == 0) {
            auto m = monitor.getCurrentMetrics();
            char buffer[256];
            sprintf(buffer, "[TICK %ds] Proc:%zu | Mem:%zuKB | SysMem:%zuMB | Files:%zu | IPC:%zu | CPU:%.1f%% | Mode:%s",
                    cycle, m.process_count, m.memory_allocated/1024, m.system_memory_usage/1024,
                    m.file_operations, m.ipc_messages, m.cpu_usage,
                    monitor.getMode() == MonitorMode::NORMAL ? "NORMAL" : "ATTACK");
            log(buffer);
        }
        
        // Simulate activity every 30 seconds
        if (cycle % 30 == 0 && cycle > 0) {
            log("");
            log("[SIM] Simulating normal system activity...");
            simulator.simulateNormalActivity();
            log("[OK] Normal activity completed");
            log("");
        }
        
        // Demo: Attack mode at 60 seconds
        if (cycle == 60) {
            log("");
            log("****************************************");
            log("[ALERT] DEMO: Switching to ATTACK mode!");
            log("[ATTACK] Simulating Fork Bomb...");
            log("****************************************");
            monitor.setMode(MonitorMode::ATTACK);
            simulator.simulateForkBomb(30);
            log("[OK] Fork Bomb simulation completed");
            log("[INFO] Check dashboard for anomaly alerts!");
            log("");
        }
        
        // Back to normal at 90 seconds
        if (cycle == 90) {
            log("");
            log("****************************************");
            log("[MODE] DEMO: Back to NORMAL mode");
            log("****************************************");
            monitor.setMode(MonitorMode::NORMAL);
            log("");
        }
        
        // Show alerts
        auto alerts = monitor.getAlerts();
        if (!alerts.empty() && cycle % 10 == 0) {
            log("");
            char buf[128];
            sprintf(buf, "[ALERT] %zu active alerts detected:", alerts.size());
            log(buf);
            for (size_t i = 0; i < std::min(alerts.size(), size_t(3)); ++i) {
                log("  -> " + alerts[i]);
            }
            log("");
        }
        
        SLEEP_MS(1000);
        cycle++;
    }
    
    logFile.close();
    return 0;
}
