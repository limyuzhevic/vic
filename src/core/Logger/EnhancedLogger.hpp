#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <functional>
#include <unordered_map>

namespace nlm {

// Enhanced logging levels
enum class EnhancedLogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Trace = -1,        // Ultra-detailed tracing
    Performance = -2,  // Performance metrics
    Memory = -3,       // Memory usage tracking
    Network = -4       // Network/communication logs
};

// Enhanced log entry with additional context
struct EnhancedLogEntry {
    EnhancedLogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::string processId;
    std::unordered_map<std::string, std::string> metadata; // Key-value pairs for structured logging
    bool isPerformanceMetric;
    double performanceValue;
    size_t memoryUsage;
    
    EnhancedLogEntry()
        : level(EnhancedLogLevel::Info), line(0), isPerformanceMetric(false),
          performanceValue(0.0), memoryUsage(0) {}
    
    EnhancedLogEntry(EnhancedLogLevel lvl, const std::string& msg, 
                     const std::string& file = "", int line = 0,
                     const std::string& function = "")
        : level(lvl), message(msg), file(file), line(line), function(function),
          isPerformanceMetric(false), performanceValue(0.0), memoryUsage(0) {
        timestamp = std::chrono::system_clock::now();
        threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    }
};

// Enhanced logger interface
class IEnhancedLogger {
public:
    virtual ~IEnhancedLogger() = default;
    virtual void log(const EnhancedLogEntry& entry) = 0;
    virtual void setLevel(EnhancedLogLevel level) = 0;
    virtual EnhancedLogLevel getLevel() const = 0;
    
    // New enhanced methods
    virtual void enableStructuredLogging(bool enable) = 0;
    virtual void setLogFormat(const std::string& format) = 0;
    virtual void addLogFilter(const std::function<bool(const EnhancedLogEntry&)>& filter) = 0;
    virtual void clearLogFilters() = 0;
};

// Enhanced console logger
class EnhancedConsoleLogger : public IEnhancedLogger {
public:
    EnhancedConsoleLogger();
    explicit EnhancedConsoleLogger(EnhancedLogLevel level);
    ~EnhancedConsoleLogger() override;
    
    void log(const EnhancedLogEntry& entry) override;
    void setLevel(EnhancedLogLevel level) override;
    EnhancedLogLevel getLevel() const override;
    
    // Enhanced console logger specific methods
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    void setTimestampFormat(const std::string& format);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(EnhancedLogLevel level) const;
    std::string levelToColor(EnhancedLogLevel level) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    std::string formatMetadata(const std::unordered_map<std::string, std::string>& metadata) const;
};

// Enhanced main logger class
class EnhancedLogger {
public:
    EnhancedLogger();
    ~EnhancedLogger();
    
    // Disable copying
    EnhancedLogger(const EnhancedLogger&) = delete;
    EnhancedLogger& operator=(const EnhancedLogger&) = delete;
    
    // Add enhanced logger sink
    void addLogger(std::shared_ptr<IEnhancedLogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(EnhancedLogLevel level);
    
    // Global logging functions
    void log(EnhancedLogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Performance and memory logging
    void logPerformance(const std::string& metric, double value, const std::string& unit = "s");
    void logMemory(size_t memoryBytes, const std::string& context = "");
    
    // Structured logging
    void logStructured(EnhancedLogLevel level, const std::string& message,
                       const std::unordered_map<std::string, std::string>& metadata,
                       const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(EnhancedLogLevel level) const;
    
    // Get global logger instance
    static EnhancedLogger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<EnhancedLogger> logger);
    
    // Error recovery and retry logic
    bool tryLog(const std::function<void()>& logOperation, int maxRetries = 3, int retryDelay = 100);
    
    // Log session management
    void startLogSession(const std::string& sessionId);
    void endLogSession(const std::string& sessionId);
    void pauseLogging();
    void resumeLogging();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced logging macros
#define NLM_ENHANCED_LOG_TRACE(msg) nlm::EnhancedLogger::getGlobal().trace(msg, __FILE__, __LINE__, __func__)
#define NLM_ENHANCED_LOG_DEBUG(msg) nlm::EnhancedLogger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_ENHANCED_LOG_INFO(msg) nlm::EnhancedLogger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_ENHANCED_LOG_WARNING(msg) nlm::EnhancedLogger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_ENHANCED_LOG_ERROR(msg) nlm::EnhancedLogger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_ENHANCED_LOG_CRITICAL(msg) nlm::EnhancedLogger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Performance logging macro
#define NLM_PERF_LOG(metric, value, unit) nlm::EnhancedLogger::getGlobal().logPerformance(metric, value, unit)

// Memory logging macro
#define NLM_MEM_LOG(bytes, context) nlm::EnhancedLogger::getGlobal().logMemory(bytes, context)

// Structured logging macro
#define NLM_STRUCT_LOG(level, msg, metadata) nlm::EnhancedLogger::getGlobal().logStructured(level, msg, metadata, __FILE__, __LINE__, __func__)

} // namespace nlm
