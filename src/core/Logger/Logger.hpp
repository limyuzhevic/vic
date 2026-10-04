#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

// Enhanced Log Severity Levels
enum class LogLevel {
    Trace = 0,           // Detailed debugging information
    Debug = 1,           // General debugging information
    Info = 2,            // General information messages
    Notice = 3,         // Important events that are not errors
    Warning = 4,         // Warning conditions that might indicate problems
    Error = 5,           // Error conditions
    Critical = 6,        // Critical errors that might cause system failures
    Alert = 7,           // Action must be taken immediately
    Emergency = 8        // System is unusable
};

// Performance metrics structure
struct PerformanceMetrics {
    size_t totalSpikes;
    size_t activeNeurons;
    size_t firingNeurons;
    float averageFiringRate;
    float excitationInhibitionRatio;
    size_t totalMemoryUsage;
    size_t workingMemoryUsage;
    size_t episodicMemoryUsage;
    float systemUtilization;
    std::chrono::milliseconds simulationTime;
    size_t processingFPS;
    
    PerformanceMetrics() : totalSpikes(0), activeNeurons(0), firingNeurons(0),
                          averageFiringRate(0.0f), excitationInhibitionRatio(0.0f),
                          totalMemoryUsage(0), workingMemoryUsage(0), episodicMemoryUsage(0),
                          systemUtilization(0.0f), simulationTime(0), processingFPS(0) {}
};

// Error category enumeration
enum class ErrorCategory {
    MemoryManagement,
    NeuralDynamics,
    Plasticity,
    MemorySystem,
    PredictionSystem,
    CognitiveFunction,
    Development,
    Neuromodulation,
    Configuration,
    I_O,
    Performance,
    Integration,
    Unknown
};

// Enhanced Log Entry structure
struct EnhancedLogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Performance metrics at log time
    PerformanceMetrics metrics;
    
    // Error information
    ErrorCategory errorCategory;
    std::string errorCode;
    std::string context;
    
    // Additional metadata
    std::unordered_map<std::string, std::string> metadata;
    
    // Performance tracking
    std::chrono::steady_clock::time_point entryCreationTime;
    size_t processingTimeMs;
    
    EnhancedLogEntry() : level(LogLevel::Info), line(0), 
                        errorCategory(ErrorCategory::Unknown), processingTimeMs(0) {
        timestamp = std::chrono::system_clock::now();
        entryCreationTime = timestamp;
    }
};

// Performance monitoring interface
class IPerformanceMonitor {
public:
    virtual ~IPerformanceMonitor() = default;
    virtual void updateMetrics(const PerformanceMetrics& metrics) = 0;
    virtual PerformanceMetrics getCurrentMetrics() const = 0;
    virtual void startProfiler(const std::string& name) = 0;
    virtual void stopProfiler(const std::string& name) = 0;
    virtual std::unordered_map<std::string, std::chrono::milliseconds> getProfileResults() const = 0;
    virtual void logPerformanceSnapshot() = 0;
    virtual size_t getTotalLogEntries() const = 0;
    virtual std::chrono::milliseconds getAverageProcessingTime() const = 0;
};

// Memory usage tracker
class MemoryTracker {
public:
    struct MemoryStats {
        size_t totalAllocated;
        size_t totalUsed;
        size_t current;
        size_t peak;
        std::unordered_map<std::string, size_t> allocationsByType;
        float utilizationRatio;
    };

    MemoryTracker();
    ~MemoryTracker();

    // Track memory allocation
    void trackAllocation(const std::string& type, size_t size, const std::string& context = "");
    void trackDeallocation(const std::string& type, size_t size);

    // Get current stats
    MemoryStats getStats() const;

    // Reset stats
    void reset();

    // Set peak alert threshold
    void setPeakThreshold(size_t thresholdBytes) { peakThreshold_ = thresholdBytes; }
    bool isPeakExceeded() const { return getStats().peak > peakThreshold_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    size_t peakThreshold_;
};

// System resource monitor
class SystemResourceMonitor {
public:
    struct ResourceStats {
        float cpuUsagePercent;
        size_t memoryTotal;
        size_t memoryUsed;
        size_t memoryAvailable;
        float memoryUsagePercent;
        size_t activeFileDescriptors;
        std::chrono::milliseconds uptime;
        size_t runningProcesses;
        double loadAverage1;
        double loadAverage5;
        double loadAverage15;
    };

    SystemResourceMonitor();
    ~SystemResourceMonitor();

    // Update resource statistics
    void update();

    // Get current stats
    ResourceStats getStats() const;

    // Check for resource thresholds
    bool isMemoryUsageHigh() const;
    bool isCpuUsageHigh() const;
    bool isFileDescriptorLimitNear() const;

    // Set thresholds
    void setMemoryUsageThreshold(float threshold) { memoryUsageThreshold_ = threshold; }
    void setCpuUsageThreshold(float threshold) { cpuUsageThreshold_ = threshold; }
    void setFileDescriptorThreshold(size_t threshold) { fdThreshold_ = threshold; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float memoryUsageThreshold_;
    float cpuUsageThreshold_;
    size_t fdThreshold_;
};

// Enhanced logger interface
class IAdvancedLogger : public ILogger {
public:
    virtual ~IAdvancedLogger() = default;
    
    // Enhanced logging methods
    virtual void logEnhanced(const EnhancedLogEntry& entry) = 0;
    
    // Performance logging
    virtual void logPerformance(const PerformanceMetrics& metrics, const std::string& context = "") = 0;
    
    // Error logging with categorization
    virtual void logError(const std::string& message, ErrorCategory category, 
                        const std::string& errorCode = "", const std::string& context = "") = 0;
    
    // Memory usage logging
    virtual void logMemoryUsage(const MemoryTracker::MemoryStats& stats) = 0;
    
    // System resource logging
    virtual void logSystemResources(const SystemResourceMonitor::ResourceStats& stats) = 0;
    
    // Performance profile logging
    virtual void logProfile(const std::string& profileName, std::chrono::milliseconds duration,
                           size_t samples = 1) = 0;
    
    // Set performance monitor
    virtual void setPerformanceMonitor(std::shared_ptr<IPerformanceMonitor> monitor) = 0;
    
    // Enable/disable performance logging
    virtual void setPerformanceLogging(bool enabled) = 0;
    virtual bool isPerformanceLoggingEnabled() const = 0;
};

// Console logger implementation with enhanced features
class ConsoleLogger : public IAdvancedLogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    // ILogger interface overrides
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // IAdvancedLogger interface
    void logEnhanced(const EnhancedLogEntry& entry) override;
    void logPerformance(const PerformanceMetrics& metrics, const std::string& context = "") override;
    void logError(const std::string& message, ErrorCategory category, 
                 const std::string& errorCode = "", const std::string& context = "") override;
    void logMemoryUsage(const MemoryTracker::MemoryStats& stats) override;
    void logSystemResources(const SystemResourceMonitor::ResourceStats& stats) override;
    void logProfile(const std::string& profileName, std::chrono::milliseconds duration,
                   size_t samples = 1) override;
    void setPerformanceMonitor(std::shared_ptr<IPerformanceMonitor> monitor) override;
    void setPerformanceLogging(bool enabled) override;
    bool isPerformanceLoggingEnabled() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Set log level filter for performance logs
    void setPerformanceLogLevel(LogLevel level);
    LogLevel getPerformanceLogLevel() const;
    
    // Enable/disable structured JSON logging
    void setStructuredLogging(bool enabled);
    bool getStructuredLogging() const;
    
    // Set maximum log file size before rotation
    void setMaxLogFileSize(size_t maxBytes);
    size_t getMaxLogFileSize() const;
    
    // Get current log file info
    std::string getLogFileInfo() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatEnhancedLog(const EnhancedLogEntry& entry) const;
    std::string formatPerformanceMetrics(const PerformanceMetrics& metrics) const;
    std::string formatErrorInfo(ErrorCategory category, const std::string& errorCode) const;
    void writeToFile(const std::string& message);
    bool shouldLogEntry(const EnhancedLogEntry& entry) const;
    std::string createLogFilename(const std::string& baseName) const;
    void rotateLogFile();
    size_t getCurrentLogFileSize() const;
};

// Main Logger class with enhanced features
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<IAdvancedLogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Set performance monitor
    void setPerformanceMonitor(std::shared_ptr<IPerformanceMonitor> monitor);
    
    // Set memory tracker
    void setMemoryTracker(std::shared_ptr<MemoryTracker> tracker);
    
    // Set system resource monitor
    void setSystemResourceMonitor(std::shared_ptr<SystemResourceMonitor> monitor);
    
    // Global logging functions (enhanced)
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    void logEnhanced(LogLevel level, const std::string& message,
                    const std::string& file = "", int line = 0,
                    const std::string& function = "",
                    const std::unordered_map<std::string, std::string>& metadata = {});
    
    void logError(LogLevel level, const std::string& message, ErrorCategory category,
                  const std::string& errorCode = "", const std::string& context = "",
                  const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Performance logging
    void logPerformance(const PerformanceMetrics& metrics, const std::string& context = "");
    void logMemoryUsage();
    void logSystemResources();
    
    // Convenience methods (enhanced)
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void notice(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void alert(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void emergency(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Get performance metrics
    PerformanceMetrics getCurrentPerformanceMetrics() const;
    
    // Get system resource stats
    SystemResourceMonitor::ResourceStats getSystemResourceStats() const;
    
    // Get memory stats
    MemoryTracker::MemoryStats getMemoryStats() const;
    
    // Get log statistics
    struct LogStats {
        size_t totalEntries;
        size_t entriesByLevel[9];  // LogLevel enum size
        std::chrono::milliseconds averageProcessingTime;
        size_t errorCount;
        size_t warningCount;
        float performanceLoggingEnabled;
        std::chrono::steady_clock::time_point sessionStartTime;
    };
    
    LogStats getLogStats() const;
    
    // Export logs to file (JSON format)
    bool exportLogs(const std::string& filepath, LogLevel minLevel = LogLevel::Trace);
    
    // Clear all logs
    void clearLogs();
    
    // Set session identifier for log correlation
    void setSessionId(const std::string& sessionId);
    std::string getSessionId() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    EnhancedLogEntry createEnhancedEntry(LogLevel level, const std::string& message,
                                        const std::string& file = "", int line = 0,
                                        const std::string& function = "",
                                        const std::unordered_map<std::string, std::string>& metadata = {});
    
    void logEnhancedInternal(const EnhancedLogEntry& entry);
    void updatePerformanceMetrics();
    void collectSystemResources();
    void collectMemoryStats();
    std::string getFormattedTimestamp(std::chrono::system_clock::time_point tp) const;
};

// Global convenience functions
inline void logPerformance(const PerformanceMetrics& metrics, const std::string& context = "") {
    Logger::getGlobal().logPerformance(metrics, context);
}

inline void logMemoryUsage() {
    Logger::getGlobal().logMemoryUsage();
}

inline void logSystemResources() {
    Logger::getGlobal().logSystemResources();
}

// Enhanced logging macros
#define NLM_LOG_ENHANCED(level, msg) nlm::Logger::getGlobal().logEnhanced(level, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_PERFORMANCE(metrics, context) nlm::Logger::getGlobal().logPerformance(metrics, context)
#define NLM_LOG_ERROR_CATEGORY(level, msg, category, errorCode, context) \
    nlm::Logger::getGlobal().logError(level, msg, category, errorCode, context, __FILE__, __LINE__, __func__)

// Stream-style enhanced logging helper
class EnhancedLogStream {
public:
    EnhancedLogStream(LogLevel level, const std::string& file, int line, const std::string& function,
                     const std::unordered_map<std::string, std::string>& metadata = {});
    ~EnhancedLogStream();
    
    template<typename T>
    EnhancedLogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add metadata
    EnhancedLogStream& withMetadata(const std::string& key, const std::string& value) {
        metadata_[key] = value;
        return *this;
    }
    
    // Set error category for errors
    EnhancedLogStream& errorCategory(ErrorCategory category) {
        errorCategory_ = category;
        return *this;
    }
    
    // Set error code for errors
    EnhancedLogStream& errorCode(const std::string& code) {
        errorCode_ = code;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::unordered_map<std::string, std::string> metadata_;
    ErrorCategory errorCategory_;
    std::string errorCode_;
};

#define NLM_LOG_ENHANCED_STREAM(level) nlm::EnhancedLogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm

// Include implementation headers
#include "Logger.cpp.implementation"  // Forward declaration to avoid circular includes
