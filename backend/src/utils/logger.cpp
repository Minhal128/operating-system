#include "logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace edr {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : min_level_(LogLevel::INFO) {
    log_file_.open("edr_system.log", std::ios::app);
}

Logger::~Logger() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < min_level_) return;
    
#ifndef USE_SIMPLE_THREADING
    std::lock_guard<std::mutex> lock(log_mutex_);
#endif
    
    std::string log_entry = "[" + getCurrentTimestamp() + "] " +
                           "[" + levelToString(level) + "] " +
                           message + "\n";
    
    std::cout << log_entry;
    
    if (log_file_.is_open()) {
        log_file_ << log_entry;
        log_file_.flush();
    }
}

void Logger::logMetrics(const SystemCallMetrics& metrics) {
#ifndef USE_SIMPLE_THREADING
    std::lock_guard<std::mutex> lock(log_mutex_);
#endif
    metrics_log_.push_back(metrics);
    
    std::ostringstream oss;
    oss << "METRIC: " << metrics.call_name 
        << " | Time: " << metrics.execution_time_ms << "ms"
        << " | Success: " << (metrics.success ? "YES" : "NO");
    
    if (!metrics.error_message.empty()) {
        oss << " | Error: " << metrics.error_message;
    }
    
    log(LogLevel::DEBUG, oss.str());
}

void Logger::exportToCSV(const std::string& filename) {
#ifndef USE_SIMPLE_THREADING
    std::lock_guard<std::mutex> lock(log_mutex_);
#endif
    
    std::ofstream csv(filename);
    if (!csv.is_open()) {
        log(LogLevel::LOG_ERROR, "Failed to open CSV file: " + filename);
        return;
    }
    
    csv << "Timestamp,CallName,ExecutionTime_ms,Success,ErrorMessage\n";
    
    for (const auto& m : metrics_log_) {
        csv << m.timestamp << ","
            << m.call_name << ","
            << m.execution_time_ms << ","
            << (m.success ? "true" : "false") << ","
            << "\"" << m.error_message << "\"\n";
    }
    
    csv.close();
    log(LogLevel::INFO, "Exported metrics to CSV: " + filename);
}

void Logger::exportToJSON(const std::string& filename) {
#ifndef USE_SIMPLE_THREADING
    std::lock_guard<std::mutex> lock(log_mutex_);
#endif
    
    std::ofstream json(filename);
    if (!json.is_open()) {
        log(LogLevel::LOG_ERROR, "Failed to open JSON file: " + filename);
        return;
    }
    
    json << "{\n  \"metrics\": [\n";
    
    for (size_t i = 0; i < metrics_log_.size(); ++i) {
        const auto& m = metrics_log_[i];
        json << "    {\n"
             << "      \"timestamp\": " << m.timestamp << ",\n"
             << "      \"call_name\": \"" << m.call_name << "\",\n"
             << "      \"execution_time_ms\": " << m.execution_time_ms << ",\n"
             << "      \"success\": " << (m.success ? "true" : "false") << ",\n"
             << "      \"error_message\": \"" << m.error_message << "\"\n"
             << "    }";
        
        if (i < metrics_log_.size() - 1) {
            json << ",";
        }
        json << "\n";
    }
    
    json << "  ]\n}\n";
    json.close();
    
    log(LogLevel::INFO, "Exported metrics to JSON: " + filename);
}

void Logger::setLogFile(const std::string& filename) {
#ifndef USE_SIMPLE_THREADING
    std::lock_guard<std::mutex> lock(log_mutex_);
#endif
    
    if (log_file_.is_open()) {
        log_file_.close();
    }
    
    log_file_.open(filename, std::ios::app);
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::LOG_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

} // namespace edr
