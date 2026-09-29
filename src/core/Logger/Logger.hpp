#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class
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
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
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

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add performance timing to log entry
    void setTiming(bool enableTiming) {
        enableTiming_ = enableTiming;
        if (enableTiming_) {
            startTime_ = std::chrono::high_resolution_clock::now();
        }
    }
    
    // Add custom metadata to log entry
    void addMetadata(const std::string& key, const std::string& value) {
        metadata_[key] = value;
    }
    
    // Get formatted log entry with timing
    std::string getFormattedEntry() const;
    
private:
    LogLevel level_;
    mutable std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    bool enableTiming_ = false;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::map<std::string, std::string> metadata_;
};

// Advanced logging macros with timing and context
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)
#define NLM_LOG_TIMED(level) nlm::LogStream(level, __FILE__, __LINE__, __func__); \
    nlm::LogStream::setTimingHelper(__func__)

// Performance timing utilities
namespace nlm {
    class PerformanceTimer {
    public:
        PerformanceTimer(const std::string& name);
        ~PerformanceTimer();
        void stop();
        double getDurationMs() const;
        
    private:
        std::string name_;
        std::chrono::high_resolution_clock::time_point startTime_;
        bool stopped_ = false;
        double durationMs_ = 0.0;
    };
}

// Structured logging helper
struct LogContext {
    std::string operation;
    std::string component;
    std::map<std::string, std::string> metadata;
    
    LogContext(const std::string& op, const std::string& comp) : operation(op), component(comp) {}
    
    template<typename T>
    LogContext& with(const std::string& key, const T& value) {
        metadata[key] = std::to_string(value);
        return *this;
    }
};

// Global logging utilities
namespace nlm {
    void logPerformanceStats();
    void exportLogs(const std::string& filepath);
    void setLogRotationSize(size_t maxFileSize);
    void enableAsyncLogging(bool enable);
}

} // namespace nlm
