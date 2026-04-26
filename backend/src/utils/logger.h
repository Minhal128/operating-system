#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <vector>

#ifdef WINDOWS_PLATFORM
    #include <windows.h>
    #define USE_SIMPLE_THREADING
#else
    #include <mutex>
#endif

#include "../core/system_calls.h"

namespace edr {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    LOG_ERROR  // Renamed to avoid Windows ERROR macro conflict
};

class Logger {
public:
    static Logger& getInstance();
    
    void log(LogLevel level, const std::string& message);
    void logMetrics(const SystemCallMetrics& metrics);
    void exportToCSV(const std::string& filename);
    void exportToJSON(const std::string& filename);
    
    void setLogFile(const std::string& filename);
    void setLogLevel(LogLevel level) { min_level_ = level; }

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    std::ofstream log_file_;
#ifndef USE_SIMPLE_THREADING
    std::mutex log_mutex_;
#endif
    LogLevel min_level_;
    std::vector<SystemCallMetrics> metrics_log_;
    
    std::string levelToString(LogLevel level);
    std::string getCurrentTimestamp();
};

} // namespace edr

#endif // LOGGER_H
