#pragma once

#include "core/Logger/Logger.hpp"
#include <string>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {
namespace utils {

// Enhanced logging system with file/line/function tracking
class EnhancedLogger {
public:
    struct LogEntry {
        LogLevel level;
        std::string message;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point timestamp;
        std::string threadId;
        
        LogEntry(LogLevel lvl, const std::string& msg, 
                 const std::string& file = "", int line = 0,
                 const std::string& function = "")
            : level(lvl), message(msg), file(file), line(line), 
              function(function), timestamp(std::chrono::system_clock::now()) {
            std::stringstream ss;
            ss << std::this_thread::get_id();
            threadId = ss.str();
        }
    };
    
    static EnhancedLogger& getInstance() {
        static EnhancedLogger instance;
        return instance;
    }
    
    void setLogLevel(LogLevel level) {
        globalLevel = level;
    }
    
    LogLevel getLogLevel() const { return globalLevel; }
    
    // Log with automatic file/line/function tracking
    template<typename... Args>
    void log(LogLevel level, const std::string& format, Args... args) {
        if (level < globalLevel && globalLevel != LogLevel::Debug) return;
        
        std::string message = formatString(format, args...);
        
        LogEntry entry(level, message, __FILE__, __LINE__, __func__);
        
        // Add to log history
        logHistory.push_back(entry);
        
        // Output to console
        outputToConsole(entry);
        
        // Limit history size
        if (logHistory.size() > maxLogHistory) {
            logHistory.erase(logHistory.begin());
        }
    }
    
    // Convenience methods
    template<typename... Args>
    void debug(const std::string& format, Args... args) {
        log(LogLevel::Debug, format, args...);
    }
    
    template<typename... Args>
    void info(const std::string& format, Args... args) {
        log(LogLevel::Info, format, args...);
    }
    
    template<typename... Args>
    void warning(const std::string& format, Args... args) {
        log(LogLevel::Warning, format, args...);
    }
    
    template<typename... Args>
    void error(const std::string& format, Args... args) {
        log(LogLevel::Error, format, args...);
    }
    
    template<typename... Args>
    void critical(const std::string& format, Args... args) {
        log(LogLevel::Critical, format, args...);
    }
    
    // Get log history for debugging
    const std::vector<LogEntry>& getLogHistory() const { return logHistory; }
    
    // Clear log history
    void clearLogHistory() { logHistory.clear(); }
    
    // Export logs to file
    bool exportLogsToFile(const std::string& filepath) const;
    
    // Filter logs by level
    std::vector<LogEntry> filterLogsByLevel(LogLevel level) const {
        std::vector<LogEntry> filtered;
        for (const auto& entry : logHistory) {
            if (entry.level == level) {
                filtered.push_back(entry);
            }
        }
        return filtered;
    }
    
    // Filter logs by time range
    std::vector<LogEntry> filterLogsByTime(
        std::chrono::system_clock::time_point start,
        std::chrono::system_clock::time_point end) const {
        std::vector<LogEntry> filtered;
        for (const auto& entry : logHistory) {
            if (entry.timestamp >= start && entry.timestamp <= end) {
                filtered.push_back(entry);
            }
        }
        return filtered;
    }
    
private:
    EnhancedLogger() : globalLevel(LogLevel::Info) {}
    
    template<typename... Args>
    std::string formatString(const std::string& format, Args... args) {
        // Simple implementation - in production use printf-style formatting
        return format; // TODO: Implement proper formatting
    }
    
    void outputToConsole(const LogEntry& entry) {
        // Format: [LEVEL] [FILE:LINE] [THREAD] Message
        std::cout << "[" << levelToString(entry.level) << "] "
                  << "[" << entry.file << ":" << entry.line << "] "
                  << "[" << entry.threadId << "] "
                  << entry.message << std::endl;
    }
    
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
    
    LogLevel globalLevel;
    std::vector<LogEntry> logHistory;
    static constexpr size_t maxLogHistory = 10000;
};

// Error handling utilities
class ErrorHandler {
public:
    enum class ErrorCode {
        None = 0,
        ConfigFileNotFound = 1,
        InvalidConfigValue = 2,
        MemoryAllocationFailed = 3,
        SimulationError = 4,
        InitializationFailed = 5,
        FileIOError = 6,
        ValidationError = 7,
        OutOfMemory = 8,
        NetworkError = 9,
        Unknown = 10
    };
    
    struct ErrorContext {
        ErrorCode code;
        std::string message;
        std::string file;
        int line;
        std::chrono::system_clock::time_point timestamp;
        std::string details;
        
        ErrorContext(ErrorCode ec, const std::string& msg, 
                     const std::string& file = "", int line = 0)
            : code(ec), message(msg), file(file), line(line), 
              timestamp(std::chrono::system_clock::now()) {}
    };
    
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    // Handle error with context
    template<typename... Args>
    void handleError(ErrorCode code, const std::string& format, Args... args) {
        std::string message = formatString(format, args...);
        ErrorContext error(code, message, __FILE__, __LINE__);
        
        // Log the error
        std::string levelStr;
        switch (code) {
            case ErrorCode::Warning: levelStr = "WARNING"; break;
            case ErrorCode::Error: levelStr = "ERROR"; break;
            case ErrorCode::Critical: levelStr = "CRITICAL"; break;
            default: levelStr = "ERROR";
        }
        
        EnhancedLogger::getInstance().log(
            (code == ErrorCode::Warning) ? LogLevel::Warning : LogLevel::Error,
            "[" + levelStr + "] " + message + " (Code: " + std::to_string(static_cast<int>(code)) + ")"
        );
        
        // Add to error history
        errorHistory.push_back(error);
        
        // Limit error history size
        if (errorHistory.size() > maxErrorHistory) {
            errorHistory.erase(errorHistory.begin());
        }
        
        // Handle critical errors differently
        if (code == ErrorCode::Critical) {
            // Could trigger system cleanup here
            onCriticalError(error);
        }
    }
    
    // Get error history
    const std::vector<ErrorContext>& getErrorHistory() const { return errorHistory; }
    
    // Clear error history
    void clearErrorHistory() { errorHistory.clear(); }
    
    // Check if specific error occurred
    bool hasError(ErrorCode code) const {
        for (const auto& error : errorHistory) {
            if (error.code == code) {
                return true;
            }
        }
        return false;
    }
    
    // Get last error of specific type
    ErrorContext getLastError(ErrorCode code) const {
        for (auto it = errorHistory.rbegin(); it != errorHistory.rend(); ++it) {
            if (it->code == code) {
                return *it;
            }
        }
        return ErrorContext(ErrorCode::None, "No error");
    }
    
    // Validate configuration value
    static bool validateConfigValue(const std::string& key, const std::string& value);
    
    // Validate simulation parameters
    static bool validateSimulationParams(const std::string& params);
    
    // Safe memory allocation with error handling
    template<typename T>
    T* safeAlloc(size_t count = 1) {
        try {
            T* ptr = new T[count];
            if (!ptr) {
                handleError(ErrorCode::MemoryAllocationFailed, 
                           "Failed to allocate memory for {} elements", count);
            }
            return ptr;
        } catch (const std::bad_alloc&) {
            handleError(ErrorCode::OutOfMemory, "Memory allocation failed for {} elements", count);
            return nullptr;
        }
    }
    
    // Safe file operations
    static bool safeFileRead(const std::string& filepath, std::string& content);
    static bool safeFileWrite(const std::string& filepath, const std::string& content);
    
private:
    ErrorHandler() = default;
    
    template<typename... Args>
    std::string formatString(const std::string& format, Args... args) {
        return format; // TODO: Implement proper formatting
    }
    
    void onCriticalError(const ErrorContext& error) {
        // Trigger emergency shutdown or recovery actions
        EnhancedLogger::getInstance().critical(
            "Critical error occurred: " + error.message + " - System may need restart"
        );
    }
    
    std::vector<ErrorContext> errorHistory;
    static constexpr size_t maxErrorHistory = 1000;
};

// System status monitor
class SystemMonitor {
public:
    struct SystemStats {
        size_t memoryUsage;        // in bytes
        size_t memoryPeak;         // peak usage
        float cpuUsage;            // percentage
        size_t threadCount;
        size_t activeConnections;
        float simulationTime;      // current simulation time
        size_t stepCount;          // number of steps executed
        float averageFiringRate;   // across all neurons
        size_t totalSpikes;        // total spikes generated
        double uptime;             // seconds since start
    };
    
    SystemMonitor() {
        startTime = std::chrono::system_clock::now();
    }
    
    void updateSystemStats(SystemStats& stats) {
        // Update statistics
        auto now = std::chrono::system_clock::now();
        stats.uptime = std::chrono::duration<double>(now - startTime).count();
        stats.stepCount = stepCount;
        stats.simulationTime = currentSimulationTime;
        stats.memoryUsage = currentMemoryUsage;
        stats.memoryPeak = peakMemoryUsage;
        stats.cpuUsage = currentCpuUsage;
        stats.threadCount = currentThreadCount;
        stats.activeConnections = activeConnections;
        stats.averageFiringRate = averageFiringRate;
        stats.totalSpikes = totalSpikes;
    }
    
    // Get current statistics
    SystemStats getCurrentStats() {
        SystemStats stats;
        updateSystemStats(stats);
        return stats;
    }
    
    // Log current system status
    void logSystemStatus() {
        SystemStats stats = getCurrentStats();
        EnhancedLogger::getInstance().info(
            "System Status - Memory: {}B (Peak: {}B), CPU: {}%, "
            "Threads: {}, Connections: {}, Steps: {}, Uptime: {}s",
            stats.memoryUsage, stats.memoryPeak, stats.cpuUsage,
            stats.threadCount, stats.activeConnections,
            stats.stepCount, stats.uptime
        );
    }
    
    // Update simulation progress
    void updateSimulationProgress(double time, size_t steps, size_t spikes) {
        currentSimulationTime = time;
        stepCount = steps;
        totalSpikes = spikes;
        
        // Update firing rate estimate
        if (steps > 0) {
            averageFiringRate = static_cast<float>(spikes) / steps;
        }
    }
    
    // Update memory usage
    void updateMemoryUsage(size_t memoryUsage) {
        currentMemoryUsage = memoryUsage;
        if (memoryUsage > peakMemoryUsage) {
            peakMemoryUsage = memoryUsage;
        }
    }
    
    // Update thread count
    void updateThreadCount(size_t threadCount) {
        currentThreadCount = threadCount;
    }
    
    // Update CPU usage
    void updateCpuUsage(float cpuUsage) {
        currentCpuUsage = cpuUsage;
    }
    
    // Update connection count
    void updateConnectionCount(size_t connections) {
        activeConnections = connections;
    }
    
private:
    std::chrono::system_clock::time_point startTime;
    size_t stepCount = 0;
    size_t totalSpikes = 0;
    double currentSimulationTime = 0.0;
    size_t currentMemoryUsage = 0;
    size_t peakMemoryUsage = 0;
    float currentCpuUsage = 0.0f;
    size_t currentThreadCount = 0;
    size_t activeConnections = 0;
    float averageFiringRate = 0.0f;
};

} // namespace utils
} // namespace nlm