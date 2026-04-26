#include "monitoring/monitor.h"
#include "simulation/attack_simulator.h"
#include "utils/logger.h"
#include "utils/performance.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

#ifdef WINDOWS_PLATFORM
    #include <windows.h>
    #include <conio.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
    
    int _kbhit() {
        struct termios oldt, newt;
        int ch;
        int oldf;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
        if(ch != EOF) {
            ungetc(ch, stdin);
            return 1;
        }
        return 0;
    }
    int _getch() { return getchar(); }
#endif

using namespace edr;

// Function to export metrics to JSON for frontend
void exportMetricsToFrontend(Monitor& monitor) {
    auto metrics = monitor.getCurrentMetrics();
    auto alerts = monitor.getAlerts();
    auto processes = monitor.getProcessManager().getActiveProcesses();
    
    std::ofstream json("../frontend/public/api/metrics.json");
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

void printStatus(const std::string& msg, const std::string& type = "INFO") {
    std::cout << "[" << type << "] " << msg << std::endl;
}

int main() {
    std::cout << "================================================\n";
    std::cout << "      EDR SYSTEM - FULL AUTOMATED BACKEND       \n";
    std::cout << "      All modes running in a single cycle       \n";
    std::cout << "================================================\n\n";

    Monitor monitor;
    AttackSimulator simulator(monitor);
    
    printStatus("Initializing System Components...", "INIT");
    monitor.start();
    printStatus("Monitoring Active.", "OK");
    printStatus("API Server Active (exporting to metrics.json)", "OK");
    printStatus("Press 'X' at any time to exit.", "HELP");
    std::cout << "------------------------------------------------\n";

    bool running = true;
    int cycle = 0;
    
    while (running) {
        // Core Monitoring
        monitor.collectMetrics();
        monitor.analyzeMetrics();
        exportMetricsToFrontend(monitor);
        
        // Automated Mode Control (The Demo Loop)
        // This ensures "all modes run together" in a sequence automatically
        
        if (cycle % 100 < 20) {
            // 0-20s: Normal Mode
            if (cycle % 100 == 0) {
                printStatus("DEMO: Starting NORMAL activity cycle...", "MODE");
                monitor.setMode(MonitorMode::NORMAL);
            }
            simulator.simulateNormalActivity();
        } 
        else if (cycle % 100 < 40) {
            // 20-40s: Fork Bomb
            if (cycle % 100 == 20) {
                printStatus("DEMO: Starting FORK BOMB attack...", "ATTACK");
                monitor.setMode(MonitorMode::ATTACK);
            }
            simulator.simulateForkBomb(20);
        }
        else if (cycle % 100 < 60) {
            // 40-60s: File Spam
            if (cycle % 100 == 40) {
                printStatus("DEMO: Starting FILE SPAM attack...", "ATTACK");
                monitor.setMode(MonitorMode::ATTACK);
            }
            simulator.simulateFileSpam(".", 100);
        }
        else if (cycle % 100 < 80) {
            // 60-80s: Memory Exhaustion
            if (cycle % 100 == 60) {
                printStatus("DEMO: Starting MEMORY EXHAUSTION attack...", "ATTACK");
                monitor.setMode(MonitorMode::ATTACK);
            }
            simulator.simulateMemoryExhaustion(50);
        }
        else {
            // 80-100s: IPC Flood
            if (cycle % 100 == 80) {
                printStatus("DEMO: Starting IPC FLOOD attack...", "ATTACK");
                monitor.setMode(MonitorMode::ATTACK);
            }
            simulator.simulateIPCFlood(2000);
        }

        // Live Console Updates
        auto m = monitor.getCurrentMetrics();
        std::cout << "[TICK " << cycle << "] Mode: " 
                  << (monitor.getMode() == MonitorMode::NORMAL ? "NORMAL" : "ATTACK") 
                  << " | Proc: " << m.process_count 
                  << " | Alerts: " << monitor.getAlerts().size() << "\r";
        std::cout.flush();

        // Check for manual exit
        if (_kbhit()) {
            if (tolower(_getch()) == 'x') running = false;
        }

        SLEEP_MS(1000);
        cycle++;
    }

    monitor.stop();
    printStatus("System Shutdown Complete.", "EXIT");
    return 0;
}
