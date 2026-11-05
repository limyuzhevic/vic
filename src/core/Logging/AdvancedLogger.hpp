// PerformanceMonitor.h - Advanced performance monitoring utilities

#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <map>
#include <mutex>
#include <sstream>
#include <fstream>

namespace nlm {

/**
 * Advanced performance monitoring class
 */
class PerformanceMonitor {
private:
    struct PerformanceData {
        std::string taskName;
        std::chrono::duration<double> duration;
        std::chrono::steady_clock::time_point startTime;
        bool isActive;
    };
    
    std::vector<PerformanceData> taskHistory_;
    std::map<std::string, double> accumulatedTime_;
    std::chrono::steady_clock::time_point sessionStart_;
    std::mutex mutex_;
    bool enabled_ = true;
    
public:
    PerformanceMonitor() {
        sessionStart_ = std::chrono::steady_clock::now();
    }
    
    ~PerformanceMonitor() {
        printReport();
    }
    
    /**
     * Start timing a task
     */
    void startTask(const std::string& taskName) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = accumulatedTime_.find(taskName);
        if (it == accumulatedTime_.end()) {
            accumulatedTime_[taskName] = 0.0;
        }
        
        taskHistory_.push_back({taskName, std::chrono::duration<double>(0), 
                               std::chrono::steady_clock::now(), true});
    }
    
    /**
     * End timing the current task
     */
    void endTask() {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        if (taskHistory_.empty() || !taskHistory_.back().isActive) {
            return;
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - taskHistory_.back().startTime);
        taskHistory_.back().duration = duration;
        taskHistory_.back().isActive = false;
        accumulatedTime_[taskHistory_.back().taskName] += duration.count();
    }
    
    /**
     * Get accumulated time for a task
     */
    double getAccumulatedTime(const std::string& taskName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = accumulatedTime_.find(taskName);
        return it != accumulatedTime_.end() ? it->second : 0.0;
    }
    
    /**
     * Get all accumulated times
     */
    std::map<std::string, double> getAllAccumulatedTimes() const {
        return accumulatedTime_;
    }
    
    /**
     * Get total session time
     */
    double getSessionTime() const {
        auto endTime = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::duration<double>>(endTime - sessionStart_).count();
    }
    
    /**
     * Print performance report
     */
    void printReport() const {
        if (!enabled_) return;
        
        std::cout << "\n=== Performance Monitor Report ===" << std::endl;
        std::cout << "Session duration: " << getSessionTime() << " seconds" << std::endl;
        std::cout << "Total tasks completed: " << taskHistory_.size() << std::endl;
        std::cout << "" << std::endl;
        
        // Print individual task timings
        std::cout << "Task Breakdown:" << std::endl;
        std::cout << std::left << std::setw(30) << "Task Name" 
                  << std::right << std::setw(15) << "Time (s)"
                  << std::right << std::setw(12) << "% of Total" << std::endl;
        std::cout << std::string(70, '=') << std::endl;
        
        double totalTime = getSessionTime();
        if (totalTime > 0) {
            for (const auto& data : taskHistory_) {
                if (!data.isActive) {
                    double percentage = (data.duration.count() / totalTime) * 100;
                    std::cout << std::left << std::setw(30) << data.taskName
                              << std::right << std::setw(15) << std::fixed << std::setprecision(4) << data.duration.count()
                              << std::right << std::setw(12) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
                }
            }
        }
        
        // Print accumulated times
        std::cout << "" << std::endl;
        std::cout << "Accumulated Times:" << std::endl;
        std::cout << std::left << std::setw(30) << "Task Name" 
                  << std::right << std::setw(15) << "Time (s)"
                  << std::right << std::setw(12) << "% of Total" << std::endl;
        std::cout << std::string(70, '=') << std::endl;
        
        for (const auto& pair : accumulatedTime_) {
            double percentage = (pair.second / totalTime) * 100;
            std::cout << std::left << std::setw(30) << pair.first
                      << std::right << std::setw(15) << std::fixed << std::setprecision(4) << pair.second
                      << std::right << std::setw(12) << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
        }
        
        std::cout << "============================================" << std::endl;
    }
    
    /**
     * Enable/disable performance monitoring
     */
    void setEnabled(bool enabled) {
        enabled_ = enabled;
    }
    
    /**
     * Check if monitoring is enabled
     */
    bool isEnabled() const {
        return enabled_;
    }
    
    /**
     * Reset session statistics
     */
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        taskHistory_.clear();
        accumulatedTime_.clear();
        sessionStart_ = std::chrono::steady_clock::now();
    }
};

/**
 * Advanced logging system with multiple output formats
 */
class AdvancedLogger {
private:
    struct LogEntry {
        std::chrono::steady_clock::time_point timestamp;
        std::string level;
        std::string message;
        std::string source;
        std::map<std::string, std::string> metadata;
    };
    
    std::vector<LogEntry> logHistory_;
    std::mutex mutex_;
    bool enabled_ = true;
    std::string logLevel_ = "INFO";
    bool logToFile_ = false;
    std::string logFile_;
    size_t maxLogEntries_ = 10000;
    
public:
    enum class LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL };
    
    AdvancedLogger() {
        // Set default log level
        setLogLevel(logLevel_);
    }
    
    ~AdvancedLogger() {
        flush();
    }
    
    /**
     * Set log level
     */
    void setLogLevel(const std::string& level) {
        logLevel_ = level;
        // Map string to enum
        if (level == "DEBUG") logLevel_ = "DEBUG";
        else if (level == "INFO") logLevel_ = "INFO";
        else if (level == "WARNING") logLevel_ = "WARNING";
        else if (level == "ERROR") logLevel_ = "ERROR";
        else if (level == "CRITICAL") logLevel_ = "CRITICAL";
        else logLevel_ = "INFO";
    }
    
    /**
     * Check if a message should be logged based on level
     */
    bool shouldLog(const std::string& level) const {
        const int levelOrder[] = {5, 4, 3, 2, 1}; // DEBUG, INFO, WARNING, ERROR, CRITICAL
        const auto& levelMap = getLevelMap();
        
        auto logIt = levelMap.find(logLevel_);
        auto msgIt = levelMap.find(level);
        
        if (logIt == levelMap.end() || msgIt == levelMap.end()) {
            return false;
        }
        
        return levelOrder[msgIt->second] >= levelOrder[logIt->second];
    }
    
    /**
     * Log a message
     */
    void log(const std::string& level, const std::string& message, const std::string& source = "") {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!shouldLog(level)) return;
        
        LogEntry entry {
            std::chrono::steady_clock::now(),
            level,
            message,
            source
        };
        
        logHistory_.push_back(entry);
        
        // Output to console
        outputToConsole(entry);
        
        // Output to file if enabled
        if (logToFile_) {
            outputToFile(entry);
        }
    }
    
    /**
     * Log debug message
     */
    void debug(const std::string& message, const std::string& source = "") {
        log("DEBUG", message, source);
    }
    
    /**
     * Log info message
     */
    void info(const std::string& message, const std::string& source = "") {
        log("INFO", message, source);
    }
    
    /**
     * Log warning message
     */
    void warning(const std::string& message, const std::string& source = "") {
        log("WARNING", message, source);
    }
    
    /**
     * Log error message
     */
    void error(const std::string& message, const std::string& source = "") {
        log("ERROR", message, source);
    }
    
    /**
     * Log critical message
     */
    void critical(const std::string& message, const std::string& source = "") {
        log("CRITICAL", message, source);
    }
    
    /**
     * Set log to file
     */
    void setLogToFile(const std::string& filename) {
        logToFile_ = true;
        logFile_ = filename;
    }
    
    /**
     * Flush log to file
     */
    void flush() {
        if (logToFile_ && !logFile_.empty()) {
            std::ofstream file(logFile_, std::ios::app);
            if (file.is_open()) {
                for (const auto& entry : logHistory_) {
                    file << getTimestamp(entry.timestamp) << " "
                         << entry.level << " "
                         << entry.source << " "
                         << entry.message << std::endl;
                }
                file.close();
            }
        }
    }
    
    /**
     * Get log history
     */
    std::vector<LogEntry> getLogHistory() const {
        return logHistory_;
    }
    
    /**
     * Get log statistics
     */
    std::map<std::string, size_t> getLogStatistics() const {
        std::map<std::string, size_t> stats;
        for (const auto& entry : logHistory_) {
            stats[entry.level]++;
        }
        return stats;
    }
    
    /**
     * Clear log history
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        logHistory_.clear();
    }
    
    /**
     * Enable/disable logging
     */
    void setEnabled(bool enabled) {
        enabled_ = enabled;
    }
    
    /**
     * Check if logging is enabled
     */
    bool isEnabled() const {
        return enabled_;
    }
    
    /**
     * Set maximum log entries
     */
    void setMaxLogEntries(size_t max) {
        maxLogEntries_ = max;
        if (logHistory_.size() > maxLogEntries_) {
            logHistory_.erase(logHistory_.begin(), logHistory_.begin() + (logHistory_.size() - maxLogEntries_));
        }
    }
    
private:
    std::map<std::string, int> getLevelMap() const {
        return {
            {"DEBUG", 0},
            {"INFO", 1},
            {"WARNING", 2},
            {"ERROR", 3},
            {"CRITICAL", 4}
        };
    }
    
    std::string getTimestamp(const std::chrono::steady_clock::time_point& tp) const {
        auto time = std::chrono::steady_clock::to_time_t(tp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%f");
        return ss.str();
    }
    
    void outputToConsole(const LogEntry& entry) {
        std::cout << "[" << getTimestamp(entry.timestamp) << "] "
                  << "[" << entry.level << "] "
                  << "[" << entry.source << "] "
                  << entry.message << std::endl;
    }
    
    void outputToFile(const LogEntry& entry) {
        std::ofstream file(logFile_, std::ios::app);
        if (file.is_open()) {
            file << getTimestamp(entry.timestamp) << " "
                 << entry.level << " "
                 << entry.source << " "
                 << entry.message << std::endl;
        }
    }
};

} // namespace nlm