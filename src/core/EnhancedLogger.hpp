#pragma once

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

namespace nlm {

// Enhanced logger with better formatting, output options, and level management
class EnhancedLogger : public virtual ILogger {
public:
    EnhancedLogger();
    ~EnhancedLogger();
    
    // Initialize logger with enhanced configuration
    bool initialize(const LoggerConfig& config);
    
    // Add log handler with enhanced properties
    void addHandler(const LogHandler& handler);
    
    // Remove log handler
    void removeHandler(size_t index);
    
    // Log entry with enhanced metadata
    void log(const LogEntry& entry);
    
    // Get all log entries
    std::vector<LogEntry> getLogEntries() const;
    
    // Clear all log entries
    void clearLogEntries();
    
    // Get statistics
    LoggerStats getStats() const;
    
    // Configure logging levels with inheritance
    void configureLogLevels(const std::unordered_map<std::string, LogLevel>& patterns);
    
    // Set minimum log level for filtering
    void setMinLogLevel(LogLevel level);
    
    // Set maximum log level for verbosity
    void setMaxLogLevel(LogLevel level);
    
    // Get log level for specific source
    LogLevel getLogLevel(const std::string& source) const;
    
    // Set log level for specific source
    void setLogLevel(const std::string& source, LogLevel level);
    
    // Export logs to file with format options
    bool exportLogs(const std::string& filepath, LogExportFormat format = LogExportFormat::TEXT);
    
    // Import logs from file
    bool importLogs(const std::string& filepath);
    
    // Get formatted log line for output
    std::string formatLogEntry(const LogEntry& entry) const;
    
    // Get log entry by level
    std::vector<LogEntry> getEntriesByLevel(LogLevel level) const;
    
    // Get log entry by source
    std::vector<LogEntry> getEntriesBySource(const std::string& source) const;
    
    // Get log entry by timestamp range
    std::vector<LogEntry> getEntriesByTimeRange(std::chrono::system_clock::time_point start,
                                               std::chrono::system_clock::time_point end) const;
    
    // Filter log entries
    std::vector<LogEntry> filterEntries(std::function<bool(const LogEntry&)> predicate) const;
    
    // Analyze log patterns
    LogAnalysis analyzePatterns() const;
    
    // Get recent log entries
    std::vector<LogEntry> getRecentEntries(size_t count = 10) const;
    
    // Get error log entries
    std::vector<LogEntry> getErrorEntries() const;
    
    // Get warning log entries
    std::vector<LogEntry> getWarningEntries() const;
    
    // Get information log entries
    std::vector<LogEntry> getInfoEntries() const;
    
    // Get debug log entries
    std::vector<LogEntry> getDebugEntries() const;
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced log handler with better formatting and filtering
class EnhancedLogHandler : public ILogger {
public:
    EnhancedLogHandler();
    ~EnhancedLogHandler();
    
    // Initialize handler with enhanced configuration
    bool initialize(const LogHandlerConfig& config);
    
    // Log with enhanced formatting
    void log(const LogEntry& entry);
    
    // Check if entry should be handled
    bool shouldHandle(const LogEntry& entry) const;
    
    // Get handler type
    LogHandlerType getType() const;
    
    // Get handler name
    std::string getName() const;
    
    // Update handler configuration
    void updateConfig(const LogHandlerConfig& config);
    
    // Get handler statistics
    HandlerStats getStats() const;
    
    // Reset statistics
    void resetStats();
    
    // Set filter function
    void setFilter(std::function<bool(const LogEntry&)> filter);
    
    // Set formatter function
    void setFormatter(std::function<std::string(const LogEntry&)> formatter);
    
    // Set processor function
    void setProcessor(std::function<void(LogEntry&)> processor);
    
    // Enable/disable handler
    void setEnabled(bool enabled);
    
    // Check if handler is enabled
    bool isEnabled() const;
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced log configuration
struct LoggerConfig {
    // Basic logger settings
    LogLevel minLevel;
    LogLevel maxLevel;
    bool enableConsole;
    bool enableFile;
    bool enableBuffer;
    bool enableNetwork;
    
    // Output configuration
    std::string consoleFormat;
    std::string fileFormat;
    std::string filePath;
    std::string fileRotationPolicy;
    size_t maxFileSizeMB;
    
    // Performance settings
    size_t maxBufferSize;
    double flushInterval;
    bool asyncLogging;
    
    // Formatting options
    bool useColors;
    bool useTimestamps;
    bool useSourceInfo;
    bool useThreadInfo;
    
    // Advanced features
    bool enableStructuredLogging;
    bool enablePerformanceMetrics;
    bool enablePatternMatching;
    std::string patternFile;
    
    // Thread safety
    bool threadSafe;
    size_t maxConcurrentThreads;
    
    LoggerConfig();
};

// Enhanced log handler configuration
struct LogHandlerConfig {
    // Handler identification
    std::string name;
    LogHandlerType type;
    
    // Output configuration
    std::string outputPath;
    std::string outputFormat;
    std::string rotationPolicy;
    size_t maxFileSizeMB;
    
    // Filtering configuration
    LogLevel minLevel;
    LogLevel maxLevel;
    std::string sourcePattern;
    std::string messagePattern;
    
    // Processing configuration
    bool enabled;
    bool async;
    std::chrono::milliseconds batchInterval;
    size_t maxBatchSize;
    
    // Formatting configuration
    bool useColors;
    bool useTimestamps;
    bool useSourceInfo;
    bool useThreadInfo;
    std::string timestampFormat;
    
    // Advanced options
    bool enableCompression;
    bool enableEncryption;
    std::string encryptionKey;
    bool enableStatistics;
    
    LogHandlerConfig();
};

// Enhanced log entry with additional metadata
struct EnhancedLogEntry : public LogEntry {
    // Additional metadata
    std::string threadId;
    std::string processId;
    double cpuUsage;
    double memoryUsage;
    std::chrono::system_clock::time_point captureTime;
    std::chrono::system::time_point processingTime;
    
    // Performance metrics
    size_t executionTimeNs;
    size_t memoryFootprintBytes;
    double contextSwitches;
    
    // Correlation information
    std::string correlationId;
    std::string spanId;
    std::string parentSpanId;
    
    // Enhanced logging information
    size_t logSequenceNumber;
    std::string logLevelString;
    std::string formattedMessage;
    std::vector<std::string> tags;
    
    EnhancedLogEntry();
    EnhancedLogEntry(const LogEntry& entry);
};

// Enhanced log statistics
struct LoggerStats {
    // Basic statistics
    size_t totalEntries;
    size_t entriesByLevel[5]; // Debug, Info, Warning, Error, Critical
    size_t entriesBySource;
    size_t entriesByThread;
    
    // Performance statistics
    double averageLogTime;
    double peakLogTime;
    size_t totalLogBytes;
    double throughputLogsPerSecond;
    
    // Error statistics
    size_t errorCount;
    size tornarErrorCount;
    size_t warningCount;
    
    // Resource usage
    double currentMemoryUsage;
    double peakMemoryUsage;
    size_t fileHandleCount;
    
    // Handler statistics
    std::vector<HandlerStats> handlerStats;
    
    LoggerStats();
};

// Handler statistics
struct HandlerStats {
    std::string handlerName;
    LogHandlerType type;
    size_t processedEntries;
    size_t filteredEntries;
    size_t errorCount;
    double averageProcessingTime;
    double memoryUsage;
    
    HandlerStats();
};

// Log analysis results
struct LogAnalysis {
    // Pattern analysis
    std::unordered_map<std::string, size_t> sourceFrequency;
    std::unordered_map<std::string, size_t> levelFrequency;
    std::unordered_map<std::string, size_t> threadFrequency;
    
    // Time-based analysis
    std::vector<std::pair<std::chrono::system_clock::time_point, size_t>> timeSeries;
    std::chrono::system_clock::time_point oldestEntry;
    std::chrono::system_clock::time_point newestEntry;
    
    // Content analysis
    std::vector<std::string> errorMessages;
    std::vector<std::string> warningMessages;
    std::vector<std::string> infoMessages;
    
    // Performance analysis
    double averageLogSize;
    double peakLogSize;
    std::vector<double> processingTimes;
    
    // Health metrics
    double logLevelRatio[5]; // Debug, Info, Warning, Error, Critical
    bool isHealthy;
    std::string healthStatus;
    
    LogAnalysis();
};

// Log export formats
enum class LogExportFormat {
    TEXT,
    JSON,
    CSV,
    XML,
    YAML,
    BINARY,
    CUSTOM
};

// Log handler types
enum class LogHandlerType {
    CONSOLE,
    FILE,
    BUFFER,
    NETWORK,
    SYSLOG,
    DATABASE,
    CUSTOM
};

// Enhanced logging manager with comprehensive management features
class EnhancedLoggingManager {
public:
    EnhancedLoggingManager();
    ~EnhancedLoggingManager();
    
    // Initialize logging manager
    bool initialize(const LoggerConfig& config);
    
    // Add enhanced logger
    bool addLogger(std::shared_ptr<EnhancedLogger> logger);
    
    // Remove enhanced logger
    bool removeLogger(size_t index);
    
    // Get enhanced logger
    std::shared_ptr<EnhancedLogger> getLogger(size_t index) const;
    
    // Get default enhanced logger
    std::shared_ptr<EnhancedLogger> getDefaultLogger() const;
    
    // Set default enhanced logger
    void setDefaultLogger(std::shared_ptr<EnhancedLogger> logger);
    
    // Get all enhanced loggers
    std::vector<std::shared_ptr<EnhancedLogger>> getAllLoggers() const;
    
    // Configure global logging
    bool configureGlobal(const LoggerConfig& config);
    
    // Get global configuration
    LoggerConfig getGlobalConfig() const;
    
    // Add global log handler
    bool addGlobalHandler(std::shared_ptr<EnhancedLogHandler> handler);
    
    // Remove global log handler
    bool removeGlobalHandler(size_t index);
    
    // Get global log handlers
    std::vector<std::shared_ptr<EnhancedLogHandler>> getGlobalHandlers() const;
    
    // Log entry with enhanced management
    void log(LogLevel level, const std::string& message,
            const std::string& file = "", int line = 0,
            const std::string& function = "", 
            const std::unordered_map<std::string, std::string>& context = {});
    
    // Log with context
    void logWithContext(LogLevel level, const std::string& message,
                       const std::unordered_map<std::string, std::string>& context,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "");
    
    // Get log statistics
    LoggerStats getStats() const;
    
    // Get analysis results
    LogAnalysis getAnalysis() const;
    
    // Export logs
    bool exportLogs(const std::string& filepath, LogExportFormat format = LogExportFormat::TEXT);
    
    // Get recent log entries
    std::vector<LogEntry> getRecentEntries(size_t count = 100) const;
    
    // Filter log entries
    std::vector<LogEntry> filterEntries(std::function<bool(const LogEntry&)> predicate) const;
    
    // Clear all logs
    void clearLogs();
    
    // Flush all handlers
    void flushAll();
    
    // Performance monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    bool isPerformanceMonitoringEnabled() const;
    
    // Health check
    bool performHealthCheck() const;
    
    // Configuration validation
    bool validateConfiguration() const;
    
    // Backup and restore
    bool backupLogs(const std::string& backupPath) const;
    bool restoreLogs(const std::string& backupPath);
    
    // Log rotation
    bool rotateLogs();
    
    // Compression
    bool compressLogs(const std::string& outputPath);
    
    // Encryption
    bool encryptLogs(const std::string& keyFile);
    
    // Advanced features
    void enableStructuredLogging();
    void disableStructuredLogging();
    bool isStructuredLoggingEnabled() const;
    
    void enablePatternMatching();
    void disablePatternMatching();
    bool isPatternMatchingEnabled() const;
    
    // Custom log levels
    void addCustomLogLevel(const std::string& name, int level);
    bool removeCustomLogLevel(const std::string& name);
    bool hasCustomLogLevel(const std::string& name) const;
    
    // Log level patterns
    void setLogLevelPattern(const std::string& pattern, LogLevel level);
    LogLevel getLogLevelForPattern(const std::string& pattern) const;
    
    // Thread-specific logging
    void setThreadLogLevel(const std::string& threadId, LogLevel level);
    LogLevel getThreadLogLevel(const std::string& threadId) const;
    
    // Performance metrics collection
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        size_t logEntriesProcessed;
        double averageLogTime;
        double throughput;
        size_t activeHandlers;
        std::chrono::system_clock::time_point lastActivity;
        
        PerformanceMetrics();
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Get all handlers statistics
    std::vector<HandlerStats> getAllHandlersStats() const;
    
    // Get system information
    std::string getSystemInfo() const;
    
    // Version information
    static std::string getVersion();
    static std::string getBuildInfo();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm