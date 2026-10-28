#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <ctime>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Structured log entry with additional metadata
struct StructuredLogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string message;
    std::string file;
    int line = 0;
    std::string function;
    std::string threadId;
    std::string processId;
    std::vector<std::pair<std::string, std::string>> metadata;  // Key-value pairs for structured logging
    
    std::string toJSON() const;
    std::string toYAML() const;
    std::string toCSV() const;
    std::string toPlainText() const;
};

// Log rotation strategy
enum class LogRotationStrategy {
    Never,
    SizeBased,
    TimeBased,
    TimeBasedWithSize
};

// Log file rotation configuration
struct LogRotationConfig {
    LogRotationStrategy strategy = LogRotationStrategy::Never;
    uint64_t maxFileSize = 10 * 1024 * 1024;  // 10MB
    uint32_t maxFiles = 5;
    std::chrono::system_clock::duration rotationInterval = std::chrono::hours(24);
    bool compressOldFiles = false;
    std::string backupExtension = ".bak";
    
    bool shouldRotate(const std::string& filepath, uint64_t fileSize) const;
};

// Advanced logger interface
class IAdvancedLogger {
public:
    virtual ~IAdvancedLogger() = default;
    
    // Basic logging interface
    virtual void log(const StructuredLogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Configuration
    virtual void setRotationConfig(const LogRotationConfig& config) = 0;
    virtual LogRotationConfig getRotationConfig() const = 0;
    
    // Advanced features
    virtual void enableStructuredOutput(bool enable) = 0;
    virtual bool isStructuredOutputEnabled() const = 0;
    
    virtual void setOutputFormat(const std::string& format) = 0;  // JSON, YAML, XML, CSV, PLAIN
    virtual std::string getOutputFormat() const = 0;
    
    // Performance features
    virtual void enableAsyncLogging(bool enable) = 0;
    virtual bool isAsyncLoggingEnabled() const = 0;
    
    // Metadata and context
    virtual void addMetadata(const std::string& key, const std::string& value) = 0;
    virtual void clearMetadata() = 0;
    virtual std::vector<std::pair<std::string, std::string>> getMetadata() const = 0;
};

// Console logger with advanced features
class AdvancedConsoleLogger : public IAdvancedLogger {
public:
    AdvancedConsoleLogger();
    explicit AdvancedConsoleLogger(LogLevel level);
    ~AdvancedConsoleLogger() override;
    
    void log(const StructuredLogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Configuration
    void setRotationConfig(const LogRotationConfig& config) override;
    LogRotationConfig getRotationConfig() const override;
    
    // Advanced features
    void enableStructuredOutput(bool enable) override;
    bool isStructuredOutputEnabled() const override;
    
    void setOutputFormat(const std::string& format) override;
    std::string getOutputFormat() const override;
    
    // Performance features
    void enableAsyncLogging(bool enable) override;
    bool isAsyncLoggingEnabled() const override;
    
    // Metadata and context
    void addMetadata(const std::string& key, const std::string& value) override;
    void clearMetadata() override;
    std::vector<std::pair<std::string, std::string>> getMetadata() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void outputToConsole(const StructuredLogEntry& entry);
    void outputToFile(const StructuredLogEntry& entry);
    std::string formatEntry(const StructuredLogEntry& entry) const;
    void rotateLogFileIfNeeded(const std::string& filepath);
};

// Main Logger class with advanced features
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
    
    // Global logging functions
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Convenience methods with metadata support
    void logWithMetadata(LogLevel level, const std::string& message,
                         const std::vector<std::pair<std::string, std::string>>& metadata,
                         const std::string& file = "", int line = 0,
                         const std::string& function = "");
    
    // Structured logging methods
    void structuredLog(LogLevel level, const std::string& message,
                       const std::vector<std::pair<std::string, std::string>>& metadata = {});
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    
    // Log statistics
    struct LogStatistics {
        uint64_t debugCount = 0;
        uint64_t infoCount = 0;
        uint64_t warningCount = 0;
        uint64_t errorCount = 0;
        uint64_t criticalCount = 0;
        uint64_t totalCount = 0;
        double avgLogTime = 0.0;
        std::chrono::system_clock::time_point firstLogTime;
        std::chrono::system_clock::time_point lastLogTime;
        
        void reset();
        std::string toString() const;
    };
    
    LogStatistics getStatistics() const;
    
    // Flush all loggers
    void flushAll();
    
    // Configuration
    void setGlobalRotationConfig(const LogRotationConfig& config);
    LogRotationConfig getGlobalRotationConfig() const;
    
    // Log file management
    void closeAllFiles();
    std::vector<std::string> getActiveLogFiles() const;
    
    // Advanced formatting
    void setLogPattern(const std::string& pattern);
    std::string getLogPattern() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for structured logging
#define NLM_LOG_STRUCT(level, msg) nlm::Logger::getGlobal().structuredLog(level, msg)
#define NLM_LOG_STRUCT_WITH_METADATA(level, msg, metadata) nlm::Logger::getGlobal().logWithMetadata(level, msg, metadata)

// Performance monitoring macros
#define NLM_PERF_START() 
#define NLM_PERF_END(msg)

// Log statistics macro
#define NLM_LOG_STATS() nlm::Logger::getGlobal().getStatistics()

// Stream-style structured logging helper
class StructuredLogStream {
public:
    StructuredLogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~StructuredLogStream();
    
    template<typename T>
    StructuredLogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    StructuredLogStream& addMetadata(const std::string& key, const std::string& value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::vector<std::pair<std::string, std::string>> metadata_;
};

#define NLM_LOG_STREAM_STRUCT(level) nlm::StructuredLogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
