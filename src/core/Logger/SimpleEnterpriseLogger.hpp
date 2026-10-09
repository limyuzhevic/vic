// Minimal NLM Enterprise Logging and Error Handling System
// A focused implementation demonstrating key enterprise features

#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <vector>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <thread>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <filesystem>

namespace nlm {

// Enhanced LogLevel enum
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Enhanced LogEntry with metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::string processId;
    std::string module;
    uint64_t correlationId;
    std::chrono::microseconds executionTime;
    std::unordered_map<std::string, std::string> fields;
};

// Exception categories and severity
enum class ExceptionCategory {
    INITIALIZATION_FAILURE,
    CONFIGURATION_ERROR,
    MEMORY_ALLOCATION_FAILURE,
    NETWORK_ERROR,
    NEURON_ERROR,
    BRAIN_ERROR,
    UNKNOWN_ERROR
};

enum class ExceptionSeverity {
    DEBUG, INFO, WARNING, ERROR, CRITICAL
};

// Forward declaration of Logger for PerformanceTimer
class Logger;

// Base NLM exception
class NLMException : public std::exception {
public:
    NLMException(const std::string& message, ExceptionCategory category,
                 ExceptionSeverity severity = ExceptionSeverity::ERROR,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "")
        : message_(message), category_(category), severity_(severity),
          file_(file), line_(line), function_(function),
          timestamp_(std::chrono::system_clock::now()) {
        
        std::ostringstream oss;
        oss << "[" << static_cast<int>(severity_) << "] " << categoryToString(category_) << ": " << message_;
        fullMessage_ = oss.str();
    }
    
    const char* what() const noexcept override { return fullMessage_.c_str(); }
    ExceptionCategory getCategory() const { return category_; }
    ExceptionSeverity getSeverity() const { return severity_; }
    
private:
    std::string categoryToString(ExceptionCategory category) {
        switch (category) {
            case ExceptionCategory::INITIALIZATION_FAILURE: return "INITIALIZATION_FAILURE";
            case ExceptionCategory::CONFIGURATION_ERROR: return "CONFIGURATION_ERROR";
            case ExceptionCategory::MEMORY_ALLOCATION_FAILURE: return "MEMORY_ALLOCATION_FAILURE";
            case ExceptionCategory::NETWORK_ERROR: return "NETWORK_ERROR";
            case ExceptionCategory::NEURON_ERROR: return "NEURON_ERROR";
            case ExceptionCategory::BRAIN_ERROR: return "BRAIN_ERROR";
            default: return "UNKNOWN";
        }
    }
    
    std::string message_;
    ExceptionCategory category_;
    ExceptionSeverity severity_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
    std::string fullMessage_;
};

// Simple performance timer
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& operation, const std::string& module = "")
        : operation_(operation), module_(module) {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
    ~PerformanceTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_);
        
        std::ostringstream oss;
        oss << module_ << " " << operation_ << " took " << duration.count() << " μs";
        
        // Get global logger (simplified approach)
        if (globalLogger_) {
            globalLogger_->info(oss.str(), "", 0, "", "Perf");
        }
    }
    
    static void setGlobalLogger(Logger* logger) { globalLogger_ = logger; }
    
private:
    std::string operation_;
    std::string module_;
    std::chrono::high_resolution_clock::time_point start_;
    static Logger* globalLogger_;
};

Logger* PerformanceTimer::globalLogger_ = nullptr;

// Simple console logger
class ConsoleLogger {
public:
    ConsoleLogger() : level_(LogLevel::Info) {}
    
    void log(const LogEntry& entry) {
        if (entry.level < level_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::ostringstream oss;
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        oss << " " << levelToString(entry.level) << " [" << entry.threadId << "] ";
        oss << entry.message;
        
        std::cout << oss.str() << std::endl;
        
        if (outputFile_.is_open()) {
            outputFile_ << oss.str() << std::endl;
        }
    }
    
    void setLevel(LogLevel level) { level_ = level; }
    LogLevel getLevel() const { return level_; }
    
    void setOutputFile(const std::string& filepath) {
        outputFile_.open(filepath, std::ios::app);
    }
    
private:
    std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
    
    std::mutex mutex_;
    LogLevel level_;
    std::ofstream outputFile_;
};

// Main Logger class
class Logger {
public:
    Logger() {
        auto consoleLogger = std::make_shared<ConsoleLogger>();
        addLogger(consoleLogger);
    }
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    void addLogger(std::shared_ptr<ConsoleLogger> logger) {
        std::lock_guard<std::mutex> lock(mutex_);
        loggers_.push_back(logger);
    }
    
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "", const std::string& module = "") {
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        LogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = getCurrentThreadId();
        entry.processId = getCurrentProcessId();
        entry.module = module;
        entry.correlationId = getCurrentCorrelationId();
        
        for (auto& logger : loggers_) {
            logger->log(entry);
        }
    }
    
    void debug(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "",
               const std::string& module = "") {
        log(LogLevel::Debug, message, file, line, function, module);
    }
    
    void info(const std::string& message, const std::string& file = "",
              int line = 0, const std::string& function = "",
              const std::string& module = "") {
        log(LogLevel::Info, message, file, line, function, module);
    }
    
    void warning(const std::string& message, const std::string& file = "",
                 int line = 0, const std::string& function = "",
                 const std::string& module = "") {
        log(LogLevel::Warning, message, file, line, function, module);
    }
    
    void error(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "",
               const std::string& module = "") {
        log(LogLevel::Error, message, file, line, function, module);
    }
    
    void critical(const std::string& message, const std::string& file = "",
                  int line = 0, const std::string& function = "",
                  const std::string& module = "") {
        log(LogLevel::Critical, message, file, line, function, module);
    }
    
    // Static methods
    static Logger& getGlobal() {
        static Logger globalLogger;
        return globalLogger;
    }
    
    static void setGlobalLogger(Logger* logger) {
        PerformanceTimer::setGlobalLogger(logger);
    }
    
    // Correlation ID management
    static uint64_t getCurrentCorrelationId() {
        static std::atomic<uint64_t> idCounter{0};
        return ++idCounter;
    }
    
    static std::string getCurrentThreadId() {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        return oss.str();
    }
    
    static std::string getCurrentProcessId() {
        return std::to_string(getpid());
    }
    
    // Performance timer helper
    PerformanceTimer startPerformanceTimer(const std::string& operation,
                                           const std::string& module = "") {
        return PerformanceTimer(operation, module);
    }
    
private:
    std::mutex mutex_;
    std::vector<std::shared_ptr<ConsoleLogger>> loggers_;
};

// Global logger instance
inline Logger& getGlobalLogger() {
    return Logger::getGlobal();
}

// Convenience macros
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Performance timing macro
#define NLM_PERFORMANCE_TIMER(operation, module) nlm::Logger::PerformanceTimer performanceTimer(operation, module)

} // namespace nlm
