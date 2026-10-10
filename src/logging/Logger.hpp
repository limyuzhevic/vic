#pragma once

#include "LogManager.hpp"
#include "LogEntry.hpp"
#include "ConsoleLogger.hpp"
#include "FileLogger.hpp"
#include "AsyncLogger.hpp"
#include "MemoryBuffer.hpp"
#include "PerformanceTracker.hpp"
#include "ErrorHandler.hpp"
#include <string>
#include <memory>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>

namespace nlm {

class Logger {
public:
    Logger() = default;
    ~Logger() {
        flush();
    }
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Global logger management
    static Logger& getGlobal() {
        static Logger globalLogger;
        return globalLogger;
    }
    
    // Core logging interface
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "") {
        LogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = getCurrentThreadId();
        
        // Record start time for performance tracking
        auto startTime = std::chrono::steady_clock::now();
        
        // Log to all configured loggers
        LogManager::getInstance().log(entry);
        
        // Add to memory buffer if enabled
        LogManager::getInstance().addMemoryEntry(entry);
        
        // Record performance
        auto endTime = std::chrono::steady_clock::now();
        std::chrono::nanoseconds duration = endTime - startTime;
        
        // Performance tracker is used by the LogManager internally
    }
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        if (isEnabled(LogLevel::TRACE)) {
            log(LogLevel::TRACE, message, file, line, function);
        }
    }
    
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        if (isEnabled(LogLevel::DEBUG)) {
            log(LogLevel::DEBUG, message, file, line, function);
        }
    }
    
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        if (isEnabled(LogLevel::INFO)) {
            log(LogLevel::INFO, message, file, line, function);
        }
    }
    
    void warn(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        if (isEnabled(LogLevel::WARN)) {
            log(LogLevel::WARN, message, file, line, function);
        }
    }
    
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        log(LogLevel::ERROR, message, file, line, function);
    }
    
    void fatal(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        log(LogLevel::FATAL, message, file, line, function);
    }
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const {
        return level >= LogManager::getInstance().getGlobalLevel();
    }
    
    // Log manager integration
    void addLogger(const std::string& name, std::shared_ptr<ILogDestination> logger) {
        LogManager::getInstance().addLogger(name, logger);
    }
    
    void removeLogger(const std::string& name) {
        LogManager::getInstance().removeLogger(name);
    }
    
    void clearLoggers() {
        LogManager::getInstance().removeAllLoggers();
    }
    
    void setLevel(LogLevel level) {
        LogManager::getInstance().setGlobalLevel(level);
    }
    
    LogLevel getLevel() const {
        return LogManager::getInstance().getGlobalLevel();
    }
    
    void flush() {
        LogManager::getInstance().flushAll();
    }
    
    // Error handling integration
    void handleError(const std::exception& e, const LogEntry& context = LogEntry()) {
        ErrorHandler::getInstance().handle(e, context);
    }
    
    // Configuration
    void configure(const std::string& configFile = "") {
        // Implement configuration loading
    }
    
    // Performance metrics
    void getPerformanceStats(std::ostream& out) const {
        // Implement performance statistics output
    }
    
    // Memory buffer
    void enableMemoryBuffer(size_t maxEntries = 10000) {
        auto buffer = std::make_shared<MemoryBufferImpl>(maxEntries);
        LogManager::getInstance().enableMemoryBuffer(buffer);
    }
    
    std::vector<LogEntry> getMemoryLogs() const {
        if (auto buffer = getMemoryBuffer()) {
            return buffer->getEntries();
        }
        return {};
    }
    
private:
    std::string getCurrentThreadId() const {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        return oss.str();
    }
    
    std::shared_ptr<MemoryBuffer> getMemoryBuffer() const {
        // This would need to be exposed from LogManager
        return nullptr;
    }
};

} // namespace nlm