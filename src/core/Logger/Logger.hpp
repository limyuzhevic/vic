#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <set>
#include <mutex>
#include <iomanip>

namespace nlm {

// Enhanced log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Enhanced metadata for brain debugging
    std::string brainId;
    std::string regionId;
    std::string neuronId;
    std::string synapseId;
    
    // Performance timing
    std::chrono::microseconds processingTime;
    
    // Structured data for advanced debugging
    std::unordered_map<std::string, std::string> structuredData;
    
    // Error context
    std::string errorCode;
    std::string errorContext;
    
    // Thread safety
    std::thread::id threadId;
};

// Log filter for selective logging
struct LogFilter {
    std::set<LogLevel> levels;
    std::vector<std::string> tags;
    std::vector<std::string> sourceFiles;
    bool includeTimestamps = true;
    bool includeDebugInfo = true;
    bool structuredOutput = false;
    
    bool matches(const LogEntry& entry) const {
        // Check log level
        if (!levels.empty() && levels.find(entry.level) == levels.end()) {
            return false;
        }
        
        // Check tags
        if (!tags.empty() && !entry.structuredData.empty()) {
            bool hasMatchingTag = false;
            for (const auto& tag : tags) {
                if (entry.structuredData.find(tag) != entry.structuredData.end()) {
                    hasMatchingTag = true;
                    break;
                }
            }
            if (!hasMatchingTag) {
                return false;
            }
        }
        
        // Check source files
        if (!sourceFiles.empty()) {
            bool hasMatchingFile = false;
            for (const auto& file : sourceFiles) {
                if (entry.file.find(file) != std::string::npos) {
                    hasMatchingFile = true;
                    break;
                }
            }
            if (!hasMatchingFile) {
                return false;
            }
        }
        
        return true;
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Filter: ";
        if (levels.empty()) oss << "All levels"; else {
            bool first = true;
            for (const auto& level : levels) {
                if (!first) oss << ", ";
                oss << static_cast<int>(level);
                first = false;
            }
        }
        if (!tags.empty()) {
            oss << ", tags: ";
            for (size_t i = 0; i < tags.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << tags[i];
            }
        }
        return oss.str();
    }
};

// Logger interface with enhanced capabilities
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced filtering and formatting
    virtual void setFilter(const LogFilter& filter) = 0;
    virtual LogFilter getFilter() const = 0;
    
    // Performance monitoring
    virtual void enablePerformanceMonitoring(bool enable) = 0;
    virtual bool isPerformanceMonitoringEnabled() const = 0;
    
    // Output formatting
    virtual void setFormat(const std::string& format) = 0;
    virtual std::string getFormat() const = 0;
    
    // Error handling
    virtual void setErrorHandler(std::function<void(const std::string&)>) = 0;
};

// Helper functions for log entry processing
std::string formatTimestamp(const std::chrono::system_clock::time_point& tp, const std::string& format = "%Y-%m-%d %H:%M:%S") {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_buf;
    localtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, format.c_str());
    return oss.str();
}

std::string sanitizeLogMessage(const std::string& message) {
    std::string result = message;
    // Remove potentially problematic characters
    result.erase(std::remove_if(result.begin(), result.end(),
        [](char c) { return c == '\r' || c == '\n'; }), result.end());
    return result;
}

// Performance logging helper
struct PerformanceTimer {
    std::string operation;
    std::chrono::system_clock::time_point start;
    
    PerformanceTimer(const std::string& op) : operation(op) {
        start = std::chrono::system_clock::now();
    }
    
    ~PerformanceTimer() {
        auto end = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "[PERF] " << operation << ": " << duration.count() << " us" << std::endl;
    }
};

// Enhanced Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced color and formatting
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setColoredOutput(bool colored);
    
    // Enhanced output file management
    void setOutputFile(const std::string& filepath);
    void appendOutputFile(const std::string& filepath);
    void closeOutputFile();
    bool isOutputFileOpen() const;
    
    // Advanced formatting
    void setTimestampFormat(const std::string& format);
    void setIncludeThreadId(bool include);
    void setIncludeProcessId(bool include);
    
    // Performance logging
    void enablePerformanceTimestamps(bool enable);
    bool isPerformanceTimestampsEnabled() const;
    
    // Filtering and formatting
    void setFilter(const LogFilter& filter) override;
    LogFilter getFilter() const override;
    void setFormat(const std::string& format) override;
    std::string getFormat() const override;
    
    // Error handling
    void setErrorHandler(std::function<void(const std::string&)> handler) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatEntry(const LogEntry& entry) const;
    void outputToConsole(const std::string& message);
    void outputToFile(const std::string& message);
};

// File logger with rotation support
class FileLogger : public ILogger {
public:
    FileLogger(const std::string& filepath);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // File management
    void setRotationSize(size_t maxBytes);
    void setRotationCount(size_t maxFiles);
    void enableCompression(bool enable);
    void setFlushInterval(std::chrono::milliseconds interval);
    
    // Log file management
    std::vector<std::string> getLogFiles() const;
    void rotateNow();
    void clearCurrentFile();
    
    // Filtering and formatting
    void setFilter(const LogFilter& filter) override;
    LogFilter getFilter() const override;
    void setFormat(const std::string& format) override;
    std::string getFormat() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance logger for timing-sensitive operations
class PerformanceLogger : public ILogger {
public:
    PerformanceLogger();
    ~PerformanceLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Performance tracking
    void startTiming(const std::string& operation);
    void endTiming(const std::string& operation);
    void logTiming(const std::string& operation, std::chrono::microseconds duration);
    
    // Profile information
    std::string generateProfileReport() const;
    void resetProfile();
    
    // Threshold logging
    void setSlowOperationThreshold(std::chrono::microseconds threshold);
    void enableThresholdLogging(bool enable);
    
    // Filtering and formatting
    void setFilter(const LogFilter& filter) override;
    LogFilter getFilter() const override;
    void setFormat(const std::string& format) override;
    std::string getFormat() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced Logger class with brain integration and performance monitoring
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Enhanced logger management
    void addLogger(std::shared_ptr<ILogger> logger);
    void removeLogger(std::shared_ptr<ILogger> logger);
    void replaceLogger(std::shared_ptr<ILogger> oldLogger, std::shared_ptr<ILogger> newLogger);
    
    // Enhanced filtering
    void setFilter(const LogFilter& filter);
    LogFilter getFilter() const;
    
    // Brain-specific logging
    void setBrainContext(const std::string& brainId, const std::string& regionId = "", const std::string& neuronId = "");
    void clearBrainContext();
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& tag = "");
    
    // Enhanced convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& tag = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& tag = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& tag = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& tag = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& tag = "");
    
    // Structured logging
    void logStructured(LogLevel level, const std::unordered_map<std::string, std::string>& data, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Performance timing
    void startPerformanceTimer(const std::string& operation);
    void endPerformanceTimer(const std::string& operation);
    std::chrono::microseconds getElapsedTime(const std::string& operation) const;
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Get log statistics
    struct LogStatistics {
        size_t totalLogs = 0;
        std::unordered_map<LogLevel, size_t> logsByLevel;
        std::unordered_map<std::string, size_t> logsByTag;
        std::chrono::microseconds totalProcessingTime = std::chrono::microseconds(0);
        std::chrono::microseconds slowestLog = std::chrono::microseconds(0);
        std::string slowestOperation;
        std::mutex statisticsMutex;
    };
    
    LogStatistics getStatistics() const;
    
    // Generate reports
    std::string generateReport() const;
    void exportLogs(const std::string& filepath);
    
    // Error handling
    void setErrorHandler(std::function<void(const std::string&)> handler);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for enhanced logging
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, "DEFAULT")
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, "DEFAULT")
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__, "DEFAULT")
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__, "DEFAULT")
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__, "DEFAULT")

// Structured logging macro
#define NLM_LOG_STRUCT(level, data) nlm::Logger::getGlobal().logStructured(level, data, __FILE__, __LINE__, __func__)

// Performance timing macros
#define NLM_PERF_START(op) nlm::PerformanceTimer perfTimer(op)
#define NLM_PERF_LOG(op) nlm::Logger::getGlobal().logTiming(op, nlm::Logger::getGlobal().getElapsedTime(op))

// Brain-specific logging macros
#define NLM_BRAIN_LOG(brain_id, msg) nlm::Logger::getGlobal().setBrainContext(brain_id); nlm::Logger::getGlobal().info(msg)
#define NLM_NEURON_LOG(neuron_id, msg) nlm::Logger::getGlobal().setBrainContext("", "", neuron_id); nlm::Logger::getGlobal().info(msg)
#define NLM_SYNAPSE_LOG(synapse_id, msg) nlm::Logger::getGlobal().setBrainContext("", "", "", synapse_id); nlm::Logger::getGlobal().info(msg)

// Error logging with context
#define NLM_LOG_ERROR_WITH_CONTEXT(error_code, error_context, msg) nlm::Logger::getGlobal().error(msg); nlm::Logger::getGlobal().logStructured(nlm::LogLevel::Error, {{ "error_code", error_code }, { "error_context", error_context }})

// Stream-style logging helper with enhanced capabilities
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function, const std::string& tag = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Structured data logging
    LogStream& operator<<(const std::pair<std::string, std::string>& pair) {
        structuredData_[pair.first] = pair.second;
        return *this;
    }
    
    // Set structured data directly
    void setStructuredData(const std::unordered_map<std::string, std::string>& data) {
        structuredData_ = data;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string tag_;
    std::unordered_map<std::string, std::string> structuredData_;
    std::chrono::system_clock::time_point startTime_;
};

#define NLM_LOG_STREAM(level, tag) nlm::LogStream(level, __FILE__, __LINE__, __func__, tag)

// Log helper functions
std::string formatTimestamp(const std::chrono::system_clock::time_point& tp, const std::string& format = "%Y-%m-%d %H:%M:%S");
std::string sanitizeLogMessage(const std::string& message);

} // namespace nlm