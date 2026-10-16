#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>

namespace nlm {

// Enhanced log severity levels with more granularity
enum class LogLevel {
    Silent = -1,      // No logging
    Emergency = 0,     // System is unusable
    Alert = 1,         // Action must be taken immediately
    Critical = 2,      // Critical conditions
    Error = 3,         // Error conditions
    Warning = 4,       // Warning conditions
    Notice = 5,        // Normal but significant conditions
    Info = 6,          // Informational messages
    Debug = 7,         // Debug level messages
    Trace = 8,         // Detailed tracing
    Verbose = 9        // Very detailed verbose logging
};

// Enhanced log entry structure with additional metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Additional metadata for structured logging
    std::unordered_map<std::string, std::string> metadata;
    std::unordered_map<std::string, double> numericMetadata;
    std::unordered_map<std::string, int> intMetadata;
    
    // Category for log filtering
    std::string category;
    
    LogEntry() : level(LogLevel::Info), line(0), timestamp(std::chrono::system_clock::now()) {}
    
    // Helper to add metadata
    template<typename T>
    void addMetadata(const std::string& key, const T& value) {
        if constexpr (std::is_same_v<T, std::string>) {
            metadata[key] = value;
        } else if constexpr (std::is_same_v<T, double>) {
            numericMetadata[key] = value;
        } else if constexpr (std::is_same_v<T, int>) {
            intMetadata[key] = value;
        }
    }
    
    // Get formatted message with metadata
    std::string getFormattedMessage() const {
        std::ostringstream oss;
        oss << "[" << logLevelToString(level) << "] ";
        
        // Add category if present
        if (!category.empty()) {
            oss << "[" << category << "] ";
        }
        
        // Add timestamp
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        oss << " " << message;
        
        // Add metadata if present
        if (!metadata.empty() || !numericMetadata.empty() || !intMetadata.empty()) {
            oss << " {";
            bool first = true;
            for (const auto& pair : metadata) {
                if (!first) oss << ", ";
                oss << pair.first << "=\"" << pair.second << "\"";
                first = false;
            }
            for (const auto& pair : numericMetadata) {
                if (!first) oss << ", ";
                oss << pair.first << "=" << pair.second;
                first = false;
            }
            for (const auto& pair : intMetadata) {
                if (!first) oss << ", ";
                oss << pair.first << "=" << pair.second;
                first = false;
            }
            oss << "}";
        }
        
        return oss.str();
    }
};

// Structured logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setFormat(const std::string& format) = 0;
    virtual std::string getFormat() const = 0;
    virtual void enableStructuredLogging(bool enable) = 0;
    virtual bool isStructuredLoggingEnabled() const = 0;
    virtual void addLogFilter(const std::string& category, LogLevel level) = 0;
    virtual void removeLogFilter(const std::string& category) = 0;
};

// Enhanced console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Formatting options
    void setFormat(const std::string& format) override;
    std::string getFormat() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Enable/disable structured logging
    void enableStructuredLogging(bool enable) override;
    bool isStructuredLoggingEnabled() const override;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Log filtering
    void addLogFilter(const std::string& category, LogLevel level) override;
    void removeLogFilter(const std::string& category) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    
    // Format helpers
    std::string formatMessage(const LogEntry& entry) const;
    bool shouldLog(const LogEntry& entry) const;
    
    // Metadata formatting
    std::string formatMetadata(const LogEntry& entry) const;
};

// Async logger implementation for high-performance logging
class AsyncLogger : public ILogger {
public:
    AsyncLogger(size_t maxQueueSize = 10000);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFormat(const std::string& format) override;
    std::string getFormat() const override;
    void enableStructuredLogging(bool enable) override;
    bool isStructuredLoggingEnabled() const override;
    void addLogFilter(const std::string& category, LogLevel level) override;
    void removeLogFilter(const std::string& category) override;
    
    // Wait for all log messages to be processed
    void flush();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
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
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Set format for all loggers
    void setFormat(const std::string& format);
    
    // Enable/disable structured logging globally
    void enableStructuredLogging(bool enable);
    
    // Global log filtering
    void addLogFilter(const std::string& category, LogLevel level);
    void removeLogFilter(const std::string& category);
    
    // Global logging functions with enhanced metadata support
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& category = "");
    
    // Convenience methods with metadata support
    void logWithMetadata(LogLevel level, const std::string& message, 
                        const std::unordered_map<std::string, std::string>& metadata,
                        const std::string& file = "", int line = 0, 
                        const std::string& function = "", const std::string& category = "");
    
    // Enhanced convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& category = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, 
              const std::string& function = "", const std::string& category = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, 
                 const std::string& function = "", const std::string& category = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& category = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, 
                  const std::string& function = "", const std::string& category = "");
    void emergency(const std::string& message, const std::string& file = "", int line = 0, 
                   const std::string& function = "", const std::string& category = "");
    void alert(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& category = "");
    void notice(const std::string& message, const std::string& file = "", int line = 0, 
                const std::string& function = "", const std::string& category = "");
    void trace(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& category = "");
    void verbose(const std::string& message, const std::string& file = "", int line = 0, 
                 const std::string& function = "", const std::string& category = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Performance monitoring
    void enablePerformanceLogging(bool enable);
    bool isPerformanceLoggingEnabled() const;
    void logPerformanceMetric(const std::string& name, double value, 
                             const std::string& category = "performance");
    
    // Memory tracking
    void enableMemoryTracking(bool enable);
    bool isMemoryTrackingEnabled() const;
    void logMemoryUsage(const std::string& category = "memory");
    
    // Get logger statistics
    struct LoggerStats {
        size_t totalLogs = 0;
        size_t logsByLevel[10] = {0}; // LogLevel::Silent to LogLevel::Verbose
        double averageLogSize = 0.0;
        double lastLogTime = 0.0;
        size_t activeLoggers = 0;
    };
    
    LoggerStats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info with category
#define NLM_LOG_EMERGENCY(msg, category) nlm::Logger::getGlobal().log(nlm::LogLevel::Emergency, msg, __FILE__, __LINE__, __func__, category)
#define NLM_LOG_ALERT(msg, category) nlm::Logger::getGlobal().log(nlm::LogLevel::Alert, msg, __FILE__, __LINE__, __func__, category)
#define NLM_LOG_CRITICAL(msg, category) nlm::Logger::getGlobal().log(nlm::LogLevel::Critical, msg, __FILE__, __LINE__, __func__, category)
#define NLM_LOG_ERROR(msg, category) nlm::Logger::getGlobal().log(nlm::LogLevel::Error, msg, __FILE__, __LINE__, __func__, category)
#define NLM_LOG_WARNING(msg, category) nlm::Logger::getGlobal().log(nlm::LogLevel::Warning, msg, __FILE__, __LINE__, __func__, category)
#define NLM_LOG_NOTICE(msg, category) nlm::Logger::getGlobal().log(nlm::LogLevel::Notice, msg, __FILE__, __LINE__, __func__, category)
#define NLM_LOG_INFO(msg, category) nlm::Logger::getGlobal().log(nlm::LogLevel::Info, msg, __FILE__, __LINE__, __func__, category)
#define NLM_LOG_DEBUG(msg, category) nlm::Logger::getGlobal().log(nlm::LogLevel::Debug, msg, __FILE__, __LINE__, __func__, category)
#define NLM_LOG_TRACE(msg, category) nlm::Logger::getGlobal().log(nlm::LogLevel::Trace, msg, __FILE__, __LINE__, __func__, category)
#define NLM_LOG_VERBOSE(msg, category) nlm::Logger::getGlobal().log(nlm::LogLevel::Verbose, msg, __FILE__, __LINE__, __func__, category)

// Stream-style logging helper with metadata support
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, 
              const std::string& function, const std::string& category = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add metadata to the log entry
    template<typename T>
    LogStream& addMetadata(const std::string& key, const T& value) {
        metadata_.addMetadata(key, value);
        return *this;
    }
    
    // Add string metadata
    LogStream& addMetadata(const std::string& key, const std::string& value) {
        metadata_.addMetadata(key, value);
        return *this;
    }
    
    // Add numeric metadata
    LogStream& addMetadata(const std::string& key, double value) {
        metadata_.addMetadata(key, value);
        return *this;
    }
    
    // Add integer metadata
    LogStream& addMetadata(const std::string& key, int value) {
        metadata_.addMetadata(key, value);
        return *this;
    }

private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string category_;
    LogEntry metadata_;
};

#define NLM_LOG_STREAM(level, category) nlm::LogStream(level, __FILE__, __LINE__, __func__, category)

// Global convenience functions for common logging patterns
namespace LogCategories {
    // Standard log categories
    const std::string SYSTEM = "system";
    const std::string BRAIN = "brain";
    const std::string MEMORY = "memory";
    const std::string NEUROMODULATION = "neuromodulation";
    const std::string COGNITION = "cognition";
    const std::string ENVIRONMENT = "environment";
    const std::string AGENT = "agent";
    const std::string PERFORMANCE = "performance";
    const std::string ERROR = "error";
    const std::string WARNING = "warning";
    const std::string DEBUG = "debug";
    const std::string TRACE = "trace";
}

} // namespace nlm
