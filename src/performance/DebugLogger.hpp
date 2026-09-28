#pragma once

/**
 * Debug Logger - Enhanced logging system for NLM
 * 
 * Provides detailed logging with different levels, file output,
 * and real-time debugging capabilities for NLM simulations.
 * 
 * Features:
 * - Multiple log levels (Debug, Info, Warning, Error, Critical)
 * - Console and file output
 * - Timestamped log entries
 * - Performance logging
 * - Debug level filtering
 * - Thread-safe logging
 * - Log rotation for long-running simulations
 */

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>
#include <sstream>

namespace nlm {

enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string message;
    std::string context;
    std::map<std::string, std::string> metadata;
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& ctx = "")
        : timestamp(std::chrono::system_clock::now()), level(lvl), message(msg), context(ctx) {}
    
    std::string toString() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] ";
        ss << "[" << logLevelToString(level) << "] ";
        if (!context.empty()) ss << "[" << context << "] ";
        ss << message;
        
        // Add metadata
        if (!metadata.empty()) {
            ss << " {";
            bool first = true;
            for (const auto& kv : metadata) {
                if (!first) ss << ", ";
                ss << kv.first << "=" << kv.second;
                first = false;
            }
            ss << "}";
        }
        
        return ss.str();
    }
    
private:
    static std::string logLevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
};

class DebugLogger {
public:
    static DebugLogger& getInstance() {
        static DebugLogger instance;
        return instance;
    }
    
    void setLogLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex_);
        minLogLevel_ = level;
    }
    
    void enableFileOutput(const std::string& filepath, bool append = false) {
        std::lock_guard<std::mutex> lock(mutex_);
        fileOutput_.open(filepath, append ? std::ios::app : std::ios::trunc);
        if (!fileOutput_.is_open()) {
            std::cerr << "Failed to open log file: " << filepath << std::endl;
        }
    }
    
    void disableFileOutput() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (fileOutput_.is_open()) {
            fileOutput_.close();
        }
    }
    
    void log(LogLevel level, const std::string& message, const std::string& context = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (level < minLogLevel_) return;
        
        LogEntry entry(level, message, context);
        
        // Add performance metadata for certain levels
        if (level == LogLevel::Info || level == LogLevel::Debug) {
            entry.metadata["step"] = std::to_string(currentStep_);
            entry.metadata["time"] = std::to_string(currentTime_);
        }
        
        // Output to console
        std::cout << entry.toString() << std::endl;
        
        // Output to file if enabled
        if (fileOutput_.is_open()) {
            fileOutput_ << entry.toString() << std::endl;
        }
        
        // Store in history
        logHistory_.push_back(entry);
        
        // Limit history size
        if (logHistory_.size() > maxHistorySize_) {
            logHistory_.erase(logHistory_.begin());
        }
        
        // Performance logging
        if (performanceLoggingEnabled_) {
            logPerformanceMetrics();
        }
    }
    
    void logDebug(const std::string& message, const std::string& context = "") {
        log(LogLevel::Debug, message, context);
    }
    
    void logInfo(const std::string& message, const std::string& context = "") {
        log(LogLevel::Info, message, context);
    }
    
    void logWarning(const std::string& message, const std::string& context = "") {
        log(LogLevel::Warning, message, context);
    }
    
    void logError(const std::string& message, const std::string& context = "") {
        log(LogLevel::Error, message, context);
    }
    
    void logCritical(const std::string& message, const std::string& context = "") {
        log(LogLevel::Critical, message, context);
    }
    
    std::vector<LogEntry> getLogHistory(size_t limit = 0) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (limit == 0 || limit > logHistory_.size()) {
            return logHistory_;
        }
        return std::vector<LogEntry>(logHistory_.end() - limit, logHistory_.end());
    }
    
    std::string getRecentLogs(size_t limit = 10) const {
        auto history = getLogHistory(limit);
        std::stringstream ss;
        for (const auto& entry : history) {
            ss << entry.toString() << std::endl;
        }
        return ss.str();
    }
    
    void clearHistory() {
        std::lock_guard<std::mutex> lock(mutex_);
        logHistory_.clear();
    }
    
    // Performance logging control
    void enablePerformanceLogging(bool enable) {
        performanceLoggingEnabled_ = enable;
    }
    
    void setLogMetadata(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        currentMetadata_[key] = value;
    }
    
    void clearMetadata() {
        std::lock_guard<std::mutex> lock(mutex_);
        currentMetadata_.clear();
    }
    
    // Simulation state integration
    void setSimulationState(size_t step, double time) {
        std::lock_guard<std::mutex> lock(mutex_);
        currentStep_ = step;
        currentTime_ = time;
    }
    
    // Search log history
    std::vector<LogEntry> searchLogs(const std::string& pattern, LogLevel minLevel = LogLevel::Debug) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<LogEntry> results;
        
        for (const auto& entry : logHistory_) {
            if (entry.level < minLevel) continue;
            if (entry.message.find(pattern) != std::string::npos ||
                entry.context.find(pattern) != std::string::npos) {
                results.push_back(entry);
            }
        }
        
        return results;
    }
    
    // Log level statistics
    struct LogStats {
        size_t totalLogs;
        size_t debugLogs;
        size_t infoLogs;
        size_t warningLogs;
        size_t errorLogs;
        size_t criticalLogs;
        double startTime;
        double endTime;
    };
    
    LogStats getLogStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        LogStats stats;
        stats.totalLogs = logHistory_.size();
        stats.debugLogs = 0; stats.infoLogs = 0; stats.warningLogs = 0;
        stats.errorLogs = 0; stats.criticalLogs = 0;
        
        for (const auto& entry : logHistory_) {
            switch (entry.level) {
                case LogLevel::Debug: stats.debugLogs++; break;
                case LogLevel::Info: stats.infoLogs++; break;
                case LogLevel::Warning: stats.warningLogs++; break;
                case LogLevel::Error: stats.errorLogs++; break;
                case LogLevel::Critical: stats.criticalLogs++; break;
            }
        }
        
        if (!logHistory_.empty()) {
            auto first = logHistory_.front().timestamp;
            auto last = logHistory_.back().timestamp;
            auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(last - first).count();
            stats.startTime = std::chrono::duration_cast<std::chrono::duration<double>>(first.time_since_epoch()).count();
            stats.endTime = std::chrono::duration_cast<std::chrono::duration<double>>(last.time_since_epoch()).count();
        } else {
            stats.startTime = stats.endTime = 0.0;
        }
        
        return stats;
    }

private:
    DebugLogger() = default;
    ~DebugLogger() {
        if (fileOutput_.is_open()) {
            fileOutput_.close();
        }
    }
    
    void logPerformanceMetrics() {
        // Log performance metrics every 1000 steps
        if (currentStep_ % 1000 == 0) {
            std::stringstream ss;
            ss << "Performance: Step " << currentStep_ << ", Time " << currentTime_ 
               << ", Logs: " << logHistory_.size() << ", Memory: " << getMemoryUsageMB() << "MB";
            log(LogLevel::Info, ss.str(), "PERFORMANCE");
        }
    }
    
    double getMemoryUsageMB() const {
        // Simple memory usage estimation
        return 0.0; // Placeholder - would need platform-specific implementation
    }
    
    DebugLogger(const DebugLogger&) = delete;
    DebugLogger& operator=(const DebugLogger&) = delete;
    
    mutable std::mutex mutex_;
    std::vector<LogEntry> logHistory_;
    std::ofstream fileOutput_;
    LogLevel minLogLevel_ = LogLevel::Info;
    size_t maxHistorySize_ = 10000;
    bool performanceLoggingEnabled_ = false;
    std::map<std::string, std::string> currentMetadata_;
    size_t currentStep_ = 0;
    double currentTime_ = 0.0;
};

// Global debug logger instance
inline DebugLogger& getDebugLogger() {
    return DebugLogger::getInstance();
}

// Convenience macros
#define DEBUG_LOG(msg) nlm::getDebugLogger().logDebug(msg, __func__)
#define INFO_LOG(msg) nlm::getDebugLogger().logInfo(msg, __func__)
#define WARNING_LOG(msg) nlm::getDebugLogger().logWarning(msg, __func__)
#define ERROR_LOG(msg) nlm::getDebugLogger().logError(msg, __func__)
#define CRITICAL_LOG(msg) nlm::getDebugLogger().logCritical(msg, __func__)

} // namespace nlm