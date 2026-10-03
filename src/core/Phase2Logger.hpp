// Phase 2 Enhanced Logging Utilities
#pragma once

#include "core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {
namespace phase2 {

/**
 * Enhanced log entry with additional metadata
 */
struct EnhancedLogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    uint64_t memoryUsage;
    double executionTime;
    std::string testName;
    int testStep;
    
    EnhancedLogEntry() : 
        level(LogLevel::Info),
        line(0),
        memoryUsage(0),
        executionTime(0.0),
        testStep(0) {}
    
    EnhancedLogEntry(LogLevel lvl, const std::string& msg, 
                    const std::string& file, int line, 
                    const std::string& function)
        : level(lvl), message(msg), file(file), line(line), 
          function(function), timestamp(std::chrono::system_clock::now()),
          memoryUsage(0), executionTime(0.0), testStep(0) {}
};

/**
 * Enhanced logger with test-specific features
 */
class Phase2Logger : public ILogger {
public:
    Phase2Logger();
    explicit Phase2Logger(LogLevel level);
    ~Phase2Logger() override;
    
    // Disable copying
    Phase2Logger(const Phase2Logger&) = delete;
    Phase2Logger& operator=(const Phase2Logger&) = delete;
    
    // ILogger interface implementation
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced logging methods
    void logEnhanced(const EnhancedLogEntry& entry);
    void info(const std::string& message, const std::string& file = "", 
             int line = 0, const std::string& function = "");
    void debug(const std::string& message, const std::string& file = "", 
              int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", 
                 int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", 
              int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", 
                  int line = 0, const std::string& function = "");
    
    // Test-specific logging
    void logTestStart(const std::string& testName, const std::string& description);
    void logTestEnd(const std::string& testName, bool passed, 
                   double executionTime, const std::string& message = "");
    void logTestStep(const std::string& testName, int step, 
                    const std::string& message);
    
    // Performance logging
    void logPerformance(const std::string& operation, double time,
                       uint64_t memoryUsage = 0);
    void logBenchmark(const std::string& benchmarkName, 
                     const PerformanceMetrics& metrics);
    
    // Configuration methods
    void setTestMode(bool enable) { testMode_ = enable; }
    bool getTestMode() const { return testMode_; }
    
    void setLogToFile(const std::string& filepath);
    void closeLogFile();
    bool isLoggingToFile() const { return logToFile_; }
    
    void setLogFormat(const std::string& format); // e.g., "standard", "json", "xml"
    void setIncludeTimestamps(bool include) { includeTimestamps_ = include; }
    void setIncludeFileInfo(bool include) { includeFileInfo_ = include; }
    void setIncludeThreadInfo(bool include) { includeThreadInfo_ = include; }
    
    // Log filtering
    void addLogFilter(LogLevel level, const std::string& pattern);
    void removeLogFilter(LogLevel level, const std::string& pattern);
    bool shouldLog(const EnhancedLogEntry& entry) const;
    
    // Statistics
    struct LogStatistics {
        uint64_t totalLogs;
        uint64_t infoLogs;
        uint64_t warningLogs;
        uint64_t errorLogs;
        uint64_t criticalLogs;
        double averageLogSize;
        std::string mostFrequentMessage;
        
        LogStatistics() : totalLogs(0), infoLogs(0), warningLogs(0), 
                         errorLogs(0), criticalLogs(0), averageLogSize(0.0) {}
    };
    
    LogStatistics getLogStatistics() const { return statistics_; }
    
    // Export functionality
    void exportLogs(const std::string& filepath, const std::string& format = "text");
    std::vector<EnhancedLogEntry> getLogs() const { return logs_; }
    void clearLogs() { logs_.clear(); }
    
    // Get global Phase2 logger instance
    static Phase2Logger& getGlobal();
    
    // Set global Phase2 logger
    static void setGlobal(std::shared_ptr<Phase2Logger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    bool testMode_;
    bool logToFile_;
    bool includeTimestamps_;
    bool includeFileInfo_;
    bool includeThreadInfo_;
    std::string logFormat_;
    std::vector<EnhancedLogEntry> logs_;
    LogStatistics statistics_;
    mutable std::mutex logMutex_;
    
    // Internal helper functions
    void initializeImpl();
    std::string formatLogEntry(const EnhancedLogEntry& entry) const;
    std::string formatLogEntryJSON(const EnhancedLogEntry& entry) const;
    std::string formatLogEntryXML(const EnhancedLogEntry& entry) const;
    std::string getCurrentThreadId() const;
    uint64_t getCurrentMemoryUsage() const;
    void updateStatistics(const EnhancedLogEntry& entry);
    bool matchesFilters(const EnhancedLogEntry& entry) const;
    std::string truncateMessage(const std::string& message, size_t maxLength = 1000) const;
};

/**
 * Logging utilities namespace
 */
namespace LoggerUtils {
    
    /**
     * Create formatted log message with test context
     */
    std::string createLogMessage(const std::string& format, 
                                 const std::vector<std::string>& args);
    
    /**
     * Parse log message for variable extraction
     */
    std::vector<std::string> parseLogMessage(const std::string& message);
    
    /**
     * Log progress for long-running operations
     */
    void logProgress(const std::string& operation, 
                    uint64_t current, uint64_t total,
                    const std::string& testName = "");
    
    /**
     * Log performance metrics
     */
    void logPerformanceMetric(const std::string& metricName, double value,
                             const std::string& unit = "", 
                             const std::string& testName = "");
    
    /**
     * Log memory usage
     */
    void logMemoryUsage(const std::string& testName = "");
    
    /**
     * Log system information
     */
    void logSystemInfo();
    
    /**
     * Generate log filename with timestamp
     */
    std::string generateLogFilename(const std::string& prefix = "log",
                                   const std::string& extension = "txt");
    
    /**
     * Validate log configuration
     */
    bool validateLogConfig(const std::string& format, 
                          const std::string& filepath);
    
    /**
     * Cleanup old log files
     */
    void cleanupOldLogs(const std::string& directory, 
                      uint32_t maxAgeDays = 30);
    
} // namespace LoggerUtils

} // namespace phase2
} // namespace nlm