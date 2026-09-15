#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <map>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <algorithm>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Structured log entry for advanced logging
struct StructuredLogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::map<std::string, std::string> fields;
    std::string threadId;
    
    // Convert to JSON string
    std::string toJSON() const;
    
    // Convert to YAML string
    std::string toYAML() const;
    
    // Convert to human-readable format
    std::string toHumanReadable() const;
};

// Log entry structure (backward compatibility)
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
};

// Logger interface with enhanced features
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Core logging methods
    virtual void log(const LogEntry& entry) = 0;
    virtual void log(const StructuredLogEntry& entry) = 0;
    
    // Level control
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Formatting options
    virtual void setFormat(const std::string& format) { }
    virtual void setTimestampFormat(const std::string& format) { }
    virtual void setUseColors(bool useColors) { }
    virtual bool getUseColors() const { return false; }
    
    // Output options
    virtual void setOutputFile(const std::string& filepath) = 0;
    virtual void closeOutputFile() = 0;
    virtual bool isFileOutputEnabled() const = 0;
    
    // Log rotation
    virtual void setMaxFileSize(size_t maxSize) { }
    virtual void setMaxFileCount(size_t maxCount) { }
    virtual void rotateLog() { }
    
    // Filtering
    virtual void addFilter(std::function<bool(const LogEntry&)>) { }
    virtual void clearFilters() { }
    
    // Special sinks
    virtual void addPatternSink(const std::string& pattern, std::function<void(const LogEntry&)>) { }
    virtual void addFieldSink(const std::string& field, std::function<void(const std::string&)>) { }
};

// Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    // Core logging interface
    void log(const LogEntry& entry) override;
    void log(const StructuredLogEntry& entry) override;
    
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors) override;
    bool getUseColors() const override;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath) override;
    void closeOutputFile() override;
    bool isFileOutputEnabled() const override;
    
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
    
    // Enhanced logging methods
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
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
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
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
